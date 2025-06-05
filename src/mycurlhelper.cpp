#include "mycurlhelper.h"
#include "logger.h"

MyCurlHelper::MyCurlHelper()
{
    curl = curl_easy_init();
    if(!curl) {
        Logger::log("Ошибка инициализации CURL", Logger::Level::ERROR);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
        data->append(ptr, size * nmemb); return size * nmemb; });
}


MyCurlHelper::~MyCurlHelper() {
    curl_easy_cleanup(curl);
}


std::string MyCurlHelper::url_encode(const std::string& str) const noexcept {
    char* encoded = curl_easy_escape(curl, str.c_str(), str.size());
    std::string result(encoded);
    curl_free(encoded);
    return result;
}


std::string MyCurlHelper::get(const std::string& url) const noexcept {
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        Logger::log(static_cast<std::string>("Ошибка сервера: ") + curl_easy_strerror(res), Logger::Level::ERROR);
    }

    return response;
}
