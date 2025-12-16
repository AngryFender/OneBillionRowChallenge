cmake_minimum_required(VERSION 3.27)
project(OneBillionRowChallenge)

set(CMAKE_CXX_STANDARD 20)

set(DATA_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data/weather_stations.csv")

find_package(Boost REQUIRED)
get_property(imported_targets DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
MESSAGE(STATUS "Imported targets ➜ ${imported_targets}")

add_executable(OneBillionRowChallenge main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	DATA_FILE_PATH=\"${DATA_FILE_PATH}\"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Boost::boost
)