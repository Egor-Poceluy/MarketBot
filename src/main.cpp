#include "marketbot.h"
#include "logger.h"
#include <thread>
#include <csignal>

bool running = true;


void signal_handler(int) {
    running = false;
}


int main() {
    std::signal(SIGINT, signal_handler);

    MarketBot bot("API_KEY");

    Logger::log("Бот включен");

    std::thread item_checker([&bot](){
        while(running) {
            bot.check_items();
            std::this_thread::sleep_for(std::chrono::minutes(3));
        }
    });

    std::thread price_checker([&bot](){
        while(running) {
            bot.check_tracked_items();
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    });

    item_checker.join();
    price_checker.join();

    Logger::log("Бот выключен");
}
