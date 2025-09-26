/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once

#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace hesiod
{

class Logger
{
public:
  // Get the singleton instance of the logger
  static std::shared_ptr<spdlog::logger> &log();

private:
  // Private constructor to prevent instantiation
  Logger() = default;

  // Disable copy constructor and assignment operator
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // Static member to hold the single instance
  static std::shared_ptr<spdlog::logger> instance;
};

} // namespace hesiod