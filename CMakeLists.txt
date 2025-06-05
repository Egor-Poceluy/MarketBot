cmake_minimum_required(VERSION 3.2)
project(MarketBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(logger STATIC src/logger.cpp)
target_include_directories(logger PUBLIC include)

add_library(my_curl_helper STATIC src/mycurlhelper.cpp)
target_include_directories(my_curl_helper PUBLIC include)
target_link_libraries(my_curl_helper PRIVATE ${CURL_LIBRARIES})

add_library(notifier STATIC src/notifier.cpp)
target_include_directories(notifier PUBLIC include)
target_link_libraries(notifier PRIVATE my_curl_helper)

add_library(marketbot_core STATIC src/marketbot.cpp)
target_include_directories(marketbot_core PUBLIC include)
target_link_libraries(marketbot_core
    PUBLIC 
	logger 
    PRIVATE 
    	notifier
	nlohmann_json::nlohmann_json 
	Threads::Threads 
	my_curl_helper
)

add_executable(marketbot src/main.cpp)
target_link_libraries(marketbot PRIVATE marketbot_core)
