#include "marketbot.h"
#include "logger.h"
#include <nlohmann/json.hpp>
#include <thread>


void MarketBot::notify_sale(const std::string& item_name, const std::string& price) const noexcept {
    notifier.send_notification(Notifier::Type::Sale, item_name, price, "level=critical&volume=10");
}


void MarketBot::notify_price_changed(const std::string& item_name, const std::string& price) const noexcept {
    notifier.send_notification(Notifier::Type::PriceChanged, item_name, price, "group=RiseInPrice&sound=multiwayinvitation");
}


void MarketBot::check_items() const noexcept {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        Logger::log("Ошибка инициализации CURL", Logger::Level::ERROR);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ("https://market.csgo.com/api/v2/items?key=" + api_key).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        Logger::log(static_cast<std::string>("Не удалось проверить инвентарь: ") + curl_easy_strerror(res), Logger::Level::ERROR);
        curl_easy_cleanup(curl);
        return;
    }

    try {
        auto json = nlohmann::json::parse(response);

        if(!json["success"].get<bool>()) {
            Logger::log("Некорректный ответ от сервера", Logger::Level::ERROR);
            curl_easy_cleanup(curl);
            return;
        }

        if(json["items"].is_null()) {
            Logger::log("У вас не выставлено ни одного предмета");
            curl_easy_cleanup(curl);
            //abort(); =)
            return;
        }

        for (const auto& item : json["items"]) {
            std::string status = item["status"].get<std::string>();

            if (status == "2") {
                std::string item_name = item["market_hash_name"].get<std::string>();
                std::string price = std::to_string(item["price"].get<double>());

                for(int i = 0; i < 10; ++i) {
                    notify_sale(item_name, price);
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::log(static_cast<std::string>("Ошибка при парсинге JSON: ") + e.what(), Logger::Level::ERROR);
        Logger::log("Ответ сервера: " + response, Logger::Level::ERROR);
    }

    curl_easy_cleanup(curl);
}


void MarketBot::check_tracked_items() const noexcept {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        Logger::log("Ошибка инициализации CURL", Logger::Level::ERROR);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);

    auto url_encode = [](CURL* curl, const std::string& str) -> std::string {
        char* encoded = curl_easy_escape(curl, str.c_str(), str.size());
        std::string result(encoded);
        curl_free(encoded);
        return result;
    };

    for(const auto& [item_name, price] : tracked_items) {
        response.clear();
        std::string encoded_item_name = url_encode(curl, item_name);

        curl_easy_setopt(curl, CURLOPT_URL, ("https://market.csgo.com/api/v2/search-item-by-hash-name?key=" + api_key + "&hash_name=" + encoded_item_name).c_str());

        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            Logger::log(static_cast<std::string>("Не удалось получить цену предмета ") + item_name + ": " + curl_easy_strerror(res), Logger::Level::ERROR);
            curl_easy_cleanup(curl);
            continue;
        }

        try {
            auto json = nlohmann::json::parse(response);

            if(!json["success"].get<bool>()) {
                Logger::log("Некорректный ответ от сервера для предмета: " + item_name, Logger::Level::ERROR);
                continue;
            }

            if(json["data"].empty()) {
                Logger::log("Сейчас предмет " + item_name + " не продается", Logger::Level::IMPORTANT);
                continue;
            }

            double current_price = json["data"][0]["price"].get<double>() / 100; // тут проблема с копейками!
            if(current_price >= price) {
                notify_price_changed(item_name, std::to_string(current_price));
            }
        } catch (std::exception& e) {
            Logger::log(static_cast<std::string>("Ошибка при парсинге JSON: ") + e.what(), Logger::Level::ERROR);
            Logger::log("Ответ сервера: " + response, Logger::Level::ERROR);
        }
    }
    curl_easy_cleanup(curl);
}


void MarketBot::add_tracked_item(const std::string& item_name, double price) noexcept {
    if(tracked_items.find(item_name) == tracked_items.end()) {
        tracked_items[item_name] = price;
        Logger::log("Предмет " + item_name + " добавлен в список отслеживаемых");
    }
    else {
        tracked_items[item_name] = price;
        Logger::log("Предмет " + item_name + " уже отслеживается", Logger::Level::IMPORTANT);
        Logger::log("Установлена новая цена - " + std::to_string(price), Logger::Level::IMPORTANT);
    }
}


void MarketBot::remove_tracked_item(const std::string& item_name) noexcept {
    if(tracked_items.find(item_name) != tracked_items.end()) {
        tracked_items.erase(item_name);
        Logger::log("Предмет " + item_name + " удален из списка отслеживаемых");
    }
    else
        Logger::log("Предмет " + item_name + " не отслеживался", Logger::Level::IMPORTANT);
}
