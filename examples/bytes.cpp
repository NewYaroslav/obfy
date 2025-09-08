#include <iostream>
#include <iomanip>
#include <obfy/obfy_bytes.hpp>

int main() {
    const unsigned char* data = OBFY_BYTES("\x01\x02\x03");
    std::cout << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < 3; ++i)
        std::cout << std::setw(2) << static_cast<int>(data[i]) << ' ';
    std::cout << std::dec << std::endl;

    auto once = OBFY_BYTES_ONCE("\x05\x06\x07");
    for (std::size_t i = 0; i < once.size(); ++i)
        std::cout << std::setw(2) << static_cast<int>(once.data()[i]) << ' ';
    std::cout << std::dec << std::endl;
}
