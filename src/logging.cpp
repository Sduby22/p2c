#include "logging.h"
#include <map>
#include <utility>

namespace logging {

std::shared_ptr<Logger> getLogger(const std::string &name) {
  auto it = Logger::_global_logger_map.find(name);
  if (it == Logger::_global_logger_map.end()) {
    auto ptr = std::make_shared<Logger>(name);
    ptr->setLevel(Logger::_global_level);
    Logger::_global_logger_map.emplace(name, ptr);
    return ptr;
  } else {
    return it->second;
  }
}

void setGlobalLevel(LEVEL level) {
  Logger::_global_level = level;
  for (auto &pair : Logger::_global_logger_map) {
    auto logger = pair.second;
    if (logger->_level.has_value())
      continue;
    logger->setLevel(level);
  }
}

} // namespace logging
