#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>

#ifdef EMSCRIPTEN
#include <sstream>
#endif

namespace logging {

using LEVEL = spdlog::level::level_enum;

namespace detail {

class _patternInit {
public:
  _patternInit(const std::string &pattern) : _pattern(pattern) {
    spdlog::set_pattern(_pattern);
  }

private:
  std::string _pattern;
};

class Logger {
public:
#ifdef EMSCRIPTEN 
  static inline std::stringstream _logstream;
  Logger(const std::string &name) { _logger = std::make_shared<spdlog::logger>(name, std::make_shared<spdlog::sinks::ostream_sink_mt>(_logstream)); };
#else
  Logger(const std::string &name) { _logger = spdlog::stderr_color_st(name); };
#endif
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
  static inline std::unordered_map<std::string, std::shared_ptr<Logger>> _loggers;

private:
  std::shared_ptr<spdlog::logger> _logger;
  inline static _patternInit _global_pattern{"[%n] %^%l%$: %v"};
};

} // namespace detail
inline void setGlobalLevel(LEVEL level) { spdlog::set_level(level); }
inline detail::Logger getLogger(const std::string &name) {
  auto it = detail::Logger::_loggers.find(name);
  if (it == detail::Logger::_loggers.end()) {
    detail::Logger::_loggers[name] = std::make_shared<detail::Logger>(name);
    return *detail::Logger::_loggers[name];
  } else {
    return *(it->second);
  }
}

#ifdef EMSCRIPTEN 
inline std::string getLog() {
  return detail::Logger::_logstream.str();
}

inline void clearLog() {
  detail::Logger::_logstream.str("");
}
#endif
} // namespace logging
