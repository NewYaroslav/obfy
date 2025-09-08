#include <iostream>
#include <obfy/obfy.hpp>

int main() {
    std::cout << OBFY_STR("hello obfy") << std::endl;
    std::cout << OBFY_STR_ONCE("temp secret").c_str() << std::endl;
    std::wcout << OBFY_WSTR(L"wide obfy") << std::endl;
}
