#include "notifier.h"
#include "logger.h"

void Notifier::send_notification(const std::string& title,
                                 const std::string& body,
                                 const std::string& flags) const noexcept {

    std::string encoded_title = my_curl_helper.url_encode(title);
    std::string encoded_body = my_curl_helper.url_encode(body);

    std::string response = my_curl_helper.get("https://api.day.app/API_KEY/"
                                              + encoded_title  + '/'
                                              + encoded_body   + '?'
                                              + flags);

    Logger::log("Уведомление отправлено");
}
