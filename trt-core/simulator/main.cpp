#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "simulator/console_logger.h"
#include "simulator/simulator_board.h"

namespace {

std::vector<uint8_t> parse_hex_line(const std::string& line) {
    std::vector<uint8_t> bytes;
    std::string cleaned;
    cleaned.reserve(line.size());

    for (char ch : line) {
        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            cleaned.push_back(ch);
        }
    }

    if (cleaned.size() % 2 != 0) {
        return {};
    }

    for (std::size_t i = 0; i < cleaned.size(); i += 2) {
        const std::string token = cleaned.substr(i, 2);
        bytes.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16)));
    }
    return bytes;
}

std::string to_hex_line(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (i > 0) {
            oss << ' ';
        }
        oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

}  // namespace

int main() {
    auto logger = std::make_shared<trt::simulator::ConsoleLogger>();
    logger->set_level(trt::core::logging::LogLevel::kTxRxDecoded);

    trt::simulator::SimulatorBoard board(logger);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        const auto request = parse_hex_line(line);
        const auto response = board.process_raw_frame(request);
        std::cout << to_hex_line(response) << '\n';
    }

    return 0;
}
