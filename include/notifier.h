#pragma once

#include <string>
#include "mycurlhelper.h"

class Notifier {
public:
    void send_notification(const std::string& item_name,
                           const std::string& price,
                           const std::string& flags = "") const noexcept;

private:
    MyCurlHelper my_curl_helper;
};
