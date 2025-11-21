cmake_minimum_required(VERSION 3.27)
project(OneBillionRowChallenge)

set(CMAKE_CXX_STANDARD 20)

set(DATA_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data/weather_stations.csv")

add_executable(OneBillionRowChallenge main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	DATA_FILE_PATH=\"${DATA_FILE_PATH}\"
)
