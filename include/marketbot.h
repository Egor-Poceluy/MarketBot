#pragma once

#include <string>

class MarketBot {
public:
    void notify(const std::string& item_name, const std::string& price, const std::string& flags) const noexcept;
    void check_items(const std::string& api_key) const noexcept;
};

