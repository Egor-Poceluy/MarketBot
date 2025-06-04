#pragma once

#include <string>

class Notifier {
public:
    void send_notification(const std::string& item_name,
                           const std::string& price,
                           const std::string& flags = "") const noexcept;
};
