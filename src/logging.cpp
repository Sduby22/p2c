#include "logging.h"
#include <unordered_map>
#include <utility>

namespace logging {

std::unordered_map<std::string, std::shared_ptr<Logger>> logger_map;
LEVEL global_level;

std::shared_ptr<Logger> getLogger(const std::string &name) {
  auto it = logger_map.find(name);
  if (it == logger_map.end()) {
    auto ptr = std::make_shared<Logger>(name);
    ptr->setLevel(global_level);
    logger_map.emplace(name, ptr);
    return ptr;
  } else {
    return it->second;
  }
}

void setGlobalLevel(LEVEL level) {
  global_level = level;
  for (auto &pair : logger_map) {
    auto logger = pair.second;
    if (logger->_level.has_value())
      continue;
    logger->setLevel(level);
  }
}

} // namespace logging
