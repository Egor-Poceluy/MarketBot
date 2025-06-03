#include "notifier.h"
#include <logger.h>
#include <curl/curl.h>

void Notifier::send_notification(Type type,
                                 const std::string& item_name,
                                 const std::string& price,
                                 const std::string& flags) const noexcept {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        Logger::log("Ошибка инициализации CURL", Logger::Level::ERROR);
        return;
    }

    std::string title = (type == Type::Sale) ? "Продажа предмета" : "Изменение цены";

    std::string body_text = (type == Type::Sale)
        ? "У вас купили " + item_name + " за " + price + " RUB"
        : "Цена на " + item_name + " изменилась: " + price + " RUB";

    auto url_encode = [](CURL* curl, const std::string& str) -> std::string {
        char* encoded = curl_easy_escape(curl, str.c_str(), str.size());
        std::string result(encoded);
        curl_free(encoded);
        return result;
    };

    std::string encoded_title = url_encode(curl, title);
    std::string encoded_body = url_encode(curl, body_text);

    std::string url = "https://api.day.app/API_KEY/"
                    + encoded_title  + '/'
                    + encoded_body   + '?'
                    + flags;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
        if (data) {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        }
        return 0;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        Logger::log(static_cast<std::string>("Уведомление не отправлено: ") + curl_easy_strerror(res), Logger::Level::ERROR);
        Logger::log("Ответ сервера: " + response, Logger::Level::ERROR);
    }
    else {
        Logger::log("Уведомление отправлено: " + body_text);
        Logger::log("Ответ сервера: " + response);
    }
    curl_easy_cleanup(curl);
}
