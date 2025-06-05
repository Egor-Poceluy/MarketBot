#include "marketbot.h"
#include "logger.h"
#include <nlohmann/json.hpp>
#include <thread>


void MarketBot::notify_sale(const std::string& item_name, const std::string& price) const noexcept {
    for(int i = 0; i != 3; ++i) {
        notifier.send_notification("Предмет продан", "У вас купили " + item_name + " за " + price + " RUB", "sound=Alarm");
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}


void MarketBot::notify_price_changed(const std::string& item_name, const std::string& price) const noexcept {
    notifier.send_notification("Цена поднялась", "Цена на " + item_name + " изменилась: " + price + " RUB", "group=RiseInPrice&sound=multiwayinvitation");
}


void MarketBot::check_items() const noexcept {
    std::string response = my_curl_helper.get("https://market.csgo.com/api/v2/items?key=" + api_key);

    try {
        auto json = nlohmann::json::parse(response);

        if(!json["success"].get<bool>()) {
            Logger::log("Отрицательный ответ от сервера: " + response, Logger::Level::ERROR);
            return;
        }

        if(json["items"].is_null()) {
            Logger::log("У вас не выставлено ни одного предмета");
            return;
        }

        for (const auto& item : json["items"]) {
            std::string status = item["status"].get<std::string>();

            if (status == "2") {
                std::string item_name = item["market_hash_name"].get<std::string>();
                std::string price = std::to_string(item["price"].get<double>());

                Logger::log("У вас купили предмет " + item_name + " за " + price, Logger::Level::IMPORTANT);
                notify_sale(item_name, price);
            }
        }
    } catch (const std::exception& e) {
        Logger::log(static_cast<std::string>("Ошибка при парсинге JSON: ") + e.what(), Logger::Level::ERROR);
        Logger::log("Ответ сервера: " + response, Logger::Level::ERROR);
    }
}


void MarketBot::check_tracked_items() const noexcept {
    for(const auto& [item_name, price] : tracked_items) {

        std::string encoded_item_name = my_curl_helper.url_encode(item_name);
        std::string response = my_curl_helper.get("https://market.csgo.com/api/v2/search-item-by-hash-name?key=" + api_key + "&hash_name=" + encoded_item_name);

        try {
            auto json = nlohmann::json::parse(response);

            if(!json["success"].get<bool>()) {
                Logger::log("Отрицательный ответ от сервера для предмета: " + item_name, Logger::Level::ERROR);
                continue;
            }

            if(json["data"].empty()) {
                Logger::log("Сейчас предмет " + item_name + " не продается");
                continue;
            }

            double current_price = json["data"][0]["price"].get<double>() / 100;

            std::stringstream formated_current_price;
            formated_current_price << std::fixed << std::setprecision(2) << current_price;

            if(current_price >= price) {
                Logger::log("Цена на предмет " + item_name + " поднялась - " + formated_current_price.str(), Logger::Level::IMPORTANT);
                notify_price_changed(item_name, formated_current_price.str());
            }
        } catch (std::exception& e) {
            Logger::log(static_cast<std::string>("Ошибка при парсинге JSON: ") + e.what(), Logger::Level::ERROR);
            Logger::log("Ответ сервера: " + response, Logger::Level::ERROR);
        }
    }
}


void MarketBot::add_tracked_item(const std::string& item_name, double price) noexcept {
    tracked_items[item_name] = price;
    Logger::log("Предмет " + item_name + " добавлен в список отслеживаемых", Logger::Level::IMPORTANT);
}


void MarketBot::remove_tracked_item(const std::string& item_name) noexcept {
    tracked_items.erase(item_name);
    Logger::log("Предмет " + item_name + " удален из списка отслеживаемых", Logger::Level::IMPORTANT);
}
