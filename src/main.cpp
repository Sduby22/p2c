#include <cstdio>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
  printf("HelloWorld\n");
  for (int i = 0; i != 3; i++) {
    spdlog::info("test info log {}", i);
    spdlog::warn("test warn log {}", i);
    spdlog::error("test error log {}", i);
  }
  return 0;
}
