cmake_minimum_required(VERSION 3.27)
project(OneBillionRowChallenge)

set(CMAKE_CXX_STANDARD 20)

# Add the flags to all build types
add_compile_options(-fno-omit-frame-pointer)

# Optional: Add detailed debug info even in Release mode
# so 'perf' can show you source code lines
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ggdb3")

set(DATA_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data/weather_stations.csv")

find_package(Boost REQUIRED)
get_property(imported_targets DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
MESSAGE(STATUS "Imported targets ➜ ${imported_targets}")

add_executable(OneBillionRowChallenge main.cpp
		iparser.h
		Strategies/istrategy.h
		mmparser.cpp
		mmparser.h
		naiveparser.cpp
		naiveparser.h
		constants.h
		data.h
		Strategies/parentthread.h
		Strategies/singlethreadspawn.h
		Strategies/multithreadspawn.h
		helper.h
		Strategies/flyweight.h
		Strategies/multithreadspawnlockfree.h
		type.h
		Strategies/multithreadspawnlockfreedoublemap.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
MESSAGE(STATUS "include directory ➜ ${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(${PROJECT_NAME} PRIVATE
	DATA_FILE_PATH=\"${DATA_FILE_PATH}\"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Boost::boost
)