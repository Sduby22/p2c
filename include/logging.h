#include "spdlog/common.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace logging {

using LEVEL = spdlog::level::level_enum;

class Logger {
public:
  Logger(const std::string &name) {
    _logger = spdlog::stderr_color_st(name);
    // [Name] level: msg
    _logger->set_pattern("[%n] %^%l%$: %v");
  };
  ~Logger() = default;
  template <typename... Args> void debug(Args &&...args) {
    _logger->debug(std::forward<Args>(args)...);
  }
  template <typename... Args> void info(Args &&...args) {
    _logger->info(std::forward<Args>(args)...);
  }
  template <typename... Args> void warn(Args &&...args) {
    _logger->warn(std::forward<Args>(args)...);
  }
  template <typename... Args> void error(Args &&...args) {
    _logger->error(std::forward<Args>(args)...);
  }
  template <typename... Args> void critical(Args &&...args) {
    _logger->critical(std::forward<Args>(args)...);
  }

  void setLevel(LEVEL level) { _logger->set_level(level); }

private:
  std::shared_ptr<spdlog::logger> _logger;
};

inline void setGlobalLevel(LEVEL level) { spdlog::set_level(level); }
inline Logger getLogger(const std::string &name) { return Logger(name); }
} // namespace logging
