#include <exception>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>

#include "Uploader.hpp"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <username> <password> <file>"
              << std::endl;
    return 0;
  }

  if (argc < 4) {
    std::cerr << "Too few arguments!" << std::endl;
    return -1;
  }

  const std::string username = argv[1];
  const std::string password = argv[2];
  const std::string filePath = argv[3];

  if (!std::filesystem::exists(filePath)) {
    std::cerr << "File not found!" << std::endl;
    return -1;
  }

  try {
    Uploader uploader(username, password);
    uploader.uploadToHome(filePath);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}