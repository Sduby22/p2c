#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace logging {

enum class LEVEL { DEBUG = 1, INFO, WARN, ERROR, CRITICAL };

class Logger {
public:
  Logger(const std::string &name) {
    _logger = spdlog::stderr_color_mt(name);
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

  void setLevel(LEVEL level) {
    _level = level;
    _logger->set_level((spdlog::level::level_enum)(int)level);
  }

private:
  inline static std::unordered_map<std::string, std::shared_ptr<Logger>> _global_logger_map;
  inline static LEVEL _global_level;
  std::shared_ptr<spdlog::logger> _logger;
  std::optional<LEVEL> _level;
  friend void setGlobalLevel(LEVEL level);
  friend std::shared_ptr<Logger> getLogger(const std::string &name);
};

void setGlobalLevel(LEVEL level);
std::shared_ptr<Logger> getLogger(const std::string &name);
} // namespace logging
