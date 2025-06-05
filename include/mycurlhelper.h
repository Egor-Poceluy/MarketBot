#pragma once

#include <curl/curl.h>
#include <string>

class MyCurlHelper
{
public:
    MyCurlHelper();
    ~MyCurlHelper();

    std::string url_encode(const std::string& str) const noexcept;
    std::string get(const std::string& url) const noexcept;
private:
    CURL* curl;
};

