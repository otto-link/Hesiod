/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"

// #define __FILE_BASENAME__  strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 :
// __FILE__

// // #define HSD_LOG "[hsd--] [" + std::string(__FILE_BASENAME__) + " " +
// std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "] " #define HSD_LOG
// std::string("[hsd--] ")

// // __FUNCTION__, __LINE__

namespace hesiod
{

// Initialize the static member
std::shared_ptr<spdlog::logger> Logger::instance = nullptr;

std::shared_ptr<spdlog::logger> &Logger::get_logger()
{
  if (!instance)
  {
    instance = spdlog::stdout_color_mt("console");
    instance->set_pattern("[hsd--] [%H:%M:%S] [%^---%L---%$] %v");
    instance->set_level(spdlog::level::trace);
  }
  return instance;
}

} // namespace hesiod
