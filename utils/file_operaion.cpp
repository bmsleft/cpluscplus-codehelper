#include <sys/stat.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

bool checkAndCreatePath(const std::string& path) {
  struct stat path_stat;
  if (stat(path.c_str(), &path_stat) == 0) {
    return true;
  } else {
    int mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    if (-1 == mkdir(path.c_str(), mode)) {
      return false;
    }
  }
  return true;
}

std::string getCurrentDateStr() {
  auto t =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::stringstream ss;
  ss << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");
  return ss.str();
}

int main(int argv, char** argc) {
  checkAndCreatePath(getCurrentDateStr());
  return 1;
}