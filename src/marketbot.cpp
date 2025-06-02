#include "marketbot.h"
#include "logger.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <thread>


void MarketBot::notify(const std::string& item_name, const std::string& price, const std::string& flags) const noexcept {
    CURL* curl = curl_easy_init();
    if (!curl) {
        Logger::log("Ошибка инициализации CURL", Logger::Level::ERROR);
        return;
    }

    std::string title = "Продажа предмета";
    std::string body_text = "У вас купили " + item_name + " за " + price + " RUB";

    auto url_encode = [](CURL* curl, const std::string& str) -> std::string {
        char* encoded = curl_easy_escape(curl, str.c_str(), str.size());
        std::string result(encoded);
        curl_free(encoded);
        return result;
    };

    std::string encoded_title = url_encode(curl, title);
    std::string encoded_body = url_encode(curl, body_text);

    std::string url = "https://api.day.app/iihFg6WXJ5SvjqTiC3Fv4g/"
                    + encoded_title  + '/'
                    + encoded_body
                    + flags;

    std::string response_data;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
        if (data) {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        }
        return 0;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        Logger::log(static_cast<std::string>("Уведомление не отправлено: ") + curl_easy_strerror(res), Logger::Level::ERROR);
        Logger::log("Ответ сервера: " + response_data, Logger::Level::ERROR);
    }
    else {
        Logger::log("Уведомление отправлено: " + body_text);
        Logger::log("Ответ сервера: " + response_data);
    }
    curl_easy_cleanup(curl);
}


void MarketBot::check_items(const std::string& api_key) const noexcept {
    CURL* curl = curl_easy_init();
    std::string answer;

    if (!curl) {
        Logger::log("Ошибка инициализации CURL", Logger::Level::ERROR);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ("https://market.csgo.com/api/v2/items?key=" + api_key).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK || answer.empty()) {
        Logger::log(static_cast<std::string>("Не удалось проверить инвентарь: ") + curl_easy_strerror(res), Logger::Level::ERROR);
        curl_easy_cleanup(curl);
        return;
    }

    try {
        auto json = nlohmann::json::parse(answer);

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
                std::string name = item["market_hash_name"].get<std::string>();
                std::string price = std::to_string(item["price"].get<int>());

                for(int i = 0; i < 10; ++i) {
                    notify(name, price, "?level=critical&volume=10");
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::log(static_cast<std::string>("Ошибка при парсинге JSON: ") + e.what(), Logger::Level::ERROR);
        Logger::log("Ответ сервера: " + answer, Logger::Level::ERROR);
    }

    curl_easy_cleanup(curl);
}
