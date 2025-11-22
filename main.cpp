#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
constexpr int LINE_SIZE = 30;
constexpr char DELIMITER = ';';

struct Data {
    double max = -DBL_MAX;
    double min = DBL_MAX;
    double mean = 0;
    size_t count = 0;
};

int main() {

    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::ifstream file(DATA_FILE_PATH);
    if (!file.is_open()) {
        std::cout << "File didn't open\n";
    }
    int total = 0;
    std::unordered_map<std::string, Data> map;
    std::string line_buffer;
    line_buffer.reserve(LINE_SIZE);
	std::stringstream ss(line_buffer);
    std::string place_buffer;
    std::string value_buffer;
    double v = 0;
    while (std::getline(file, line_buffer)) {
        v = 0;
        ss.clear();
        ss.str(line_buffer);
        if (std::getline(ss, place_buffer, DELIMITER) && std::getline(ss, value_buffer, DELIMITER)) {
            auto& data = map[place_buffer];
            try {
                v = std::stod(value_buffer);
                data.min = std::min(v, data.min);
                data.max = std::max(v, data.max);
                data.mean += (v - data.mean) / ++data.count;
            }catch(std::exception& ex){}
        }
        ++total;
    };

    for (const auto& [key,data] : map) {
		std::cout << key << "," << data.min << "," << data.mean << "," << data.max << "," << data.count << "\n";
    }
    std::cout << "\nTotal = " << total << "\n";
    auto end_time = std::chrono::high_resolution_clock::now();
	auto diff_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Time take =" << diff_time << " milliseconds\n";
    return 0;
}
