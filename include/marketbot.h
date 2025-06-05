#pragma once

#include <string>
#include <map>
#include "mycurlhelper.h"
#include "notifier.h"


class MarketBot {
public:
    MarketBot(const std::string& market_api_key)
        : api_key(market_api_key) {}

    void notify_sale(const std::string& item, const std::string& price) const noexcept;
    void notify_price_changed(const std::string& item, const std::string& price) const noexcept;

    void check_items() const noexcept;
    void check_tracked_items() const noexcept;

    void add_tracked_item(const std::string& item_name, double price) noexcept;
    void remove_tracked_item(const std::string& item_name) noexcept;

private:
    std::map<std::string, double> tracked_items;
    std::string api_key;

    Notifier notifier;
    MyCurlHelper my_curl_helper;
};

