#pragma once

#include <string>

class Logger {
public:
    enum class Level { WARNING, ERROR, INFO, IMPORTANT };

    static void log(const std::string &message, Level level = Level::INFO) noexcept;
    inline static void switch_colors(bool decision) noexcept;

private:
    inline static std::string level_to_str(Level level) noexcept;
    inline static const char* level_to_color(Level level) noexcept;

    static constexpr const char* RESET  = "\033[0m";
    static constexpr const char* RED    = "\033[31m";
    static constexpr const char* YELLOW = "\033[33m";
    static constexpr const char* WHITE = "\033[37m";
    static constexpr const char* LIGHT_BLUE = "\033[1;34m";

    static inline bool colors_enabled = true;
};
