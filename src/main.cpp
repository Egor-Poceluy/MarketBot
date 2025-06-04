#include "marketbot.h"
#include "logger.h"
#include <thread>


int main() {
    MarketBot bot1("API_KEY1");
    MarketBot bot2("API_KEY2");

    Logger::log("Бот включен");


    std::thread t1([&bot1](){
        while(true) {
            bot1.check_items();
            Logger::log("Бот 1 проверил инвентарь");
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });

    std::thread t2([&bot2](){
        while(true) {
            bot2.check_items();
            Logger::log("Бот 2 проверил инвентарь");
            std::this_thread::sleep_for(std::chrono::minutes(1));

        }
    });

    t1.join();
    t2.join();
}
