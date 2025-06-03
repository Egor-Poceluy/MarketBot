#pragma once

#include <string>

class Notifier {
public:
    enum class Type{
        Sale,
        PriceChanged
    };

    void send_notification(Type type,
                           const std::string& item_name,
                           const std::string& price,
                           const std::string& flags = "") const noexcept;
};
