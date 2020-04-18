// 封装系统命令调用获取结果
// 使用智能指针，将文件的关闭动作当作析构函数传入

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

std::string exec(const std::string& cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"),
                                                pclose);
  if (!pipe) {
    return "";
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

int main() {
  std::string cmdStr = "df | grep dev | awk 'NR==1 {print $4}'";
  std::cout << "bms: " << exec(cmdStr);
  return 0;
}