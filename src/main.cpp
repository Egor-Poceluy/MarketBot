#include "marketbot.h"
#include "logger.h"
#include <thread>

int main() {
    //setlocale(LC_ALL, "ru");
    MarketBot bot;

    Logger::log("Бот включен");
    while(true) {
        bot.check_items("API_KEY");
        Logger::log("Проверил инвентарь");
        std::this_thread::sleep_for(std::chrono::minutes(3));
    }
}
