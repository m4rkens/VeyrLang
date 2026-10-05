#include <veyr/build_info.hpp>

#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
  if (argc == 2 && std::string_view{argv[1]} == "--version") {
    std::cout << "veyrc Veyr " << veyr::build_version() << '\n';
    return std::cout ? 0 : 1;
  }
  std::cerr << "Unsupported arguments. Usage: veyrc --version\n";
  return 2;
}
