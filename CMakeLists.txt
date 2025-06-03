cmake_minimum_required(VERSION 3.2)
project(MarketBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(logger STATIC src/logger.cpp)
target_include_directories(logger PUBLIC include)

add_library(notifier STATIC src/notifier.cpp)
target_include_directories(notifier PUBLIC include)

add_library(marketbot_core STATIC src/marketbot.cpp)
target_include_directories(marketbot_core PUBLIC include)

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(marketbot_core
    PUBLIC 
	logger 
    PRIVATE 
    	notifier
	nlohmann_json::nlohmann_json 
	Threads::Threads 
	${CURL_LIBRARIES}
)

add_executable(marketbot src/main.cpp)
target_link_libraries(marketbot PRIVATE marketbot_core)
