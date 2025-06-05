#include "logger.h"

#include <iostream>
#include <chrono>
#include <iomanip>

void Logger::log(const std::string &message, Level level) noexcept {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);

    std::lock_guard<std::mutex> lock(mutex);

    std::cout << level_to_color(level)
              << '[' << std::put_time(std::localtime(&now_time), "%T") << ']'
              << '[' << level_to_str(level) << "] "
              << message
              << (colors_enabled ? RESET : "")
              << std::endl;
}


std::string Logger::level_to_str(Level level) noexcept {
    switch(level) {
        case Level::INFO:       return "INFO";
        case Level::WARNING:    return "WARNING";
        case Level::ERROR:      return "ERROR";
        case Level::IMPORTANT:  return "IMPORTANT";
        default:                return "NOTYPE";
    }
}


const char* Logger::level_to_color(Level level) noexcept {
    if(!colors_enabled) return "";

    switch(level) {
        case Level::INFO:       return WHITE;
        case Level::WARNING:    return YELLOW;
        case Level::ERROR:      return RED;
        case Level::IMPORTANT:  return LIGHT_BLUE;
        default:                return RESET;
    }
}


void Logger::switch_colors(bool decision) noexcept {
    colors_enabled = decision;
}
