/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <chrono>
#include <fstream>
#include <iomanip>

#include "nlohmann/json.hpp"

#include "hesiod/logger.hpp"

namespace hesiod
{

nlohmann::json json_from_file(const std::string &fname)
{
  nlohmann::json json;
  std::ifstream  file(fname);

  if (file.is_open())
  {
    file >> json;
    file.close();
    LOG->trace("json_from_file: JSON successfully loaded from {}", fname);
  }
  else
  {
    LOG->error("json_from_file: Could not open file {} to load JSON", fname);
  }

  return json;
}

void json_to_file(const nlohmann::json &json, const std::string &fname)
{
  std::ofstream file(fname);

  if (file.is_open())
  {
    file << json.dump(4);
    file.close();
    LOG->trace("json_to_file: JSON successfully written to {}", fname);
  }
  else
  {
    LOG->error("json_to_file: Could not open file {} to save JSON", fname);
  }
}

std::vector<std::string> split_string(const std::string &string, char delimiter)
{
  std::vector<std::string> result;
  std::stringstream        ss(string);
  std::string              word;

  while (std::getline(ss, word, delimiter))
    result.push_back(word);

  return result;
}

std::string time_stamp()
{
  auto              now = std::chrono::system_clock::now();
  std::time_t       now_c = std::chrono::system_clock::to_time_t(now);
  std::stringstream time_stream;
  time_stream << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H:%M:%S");
  return time_stream.str();
}

} // namespace hesiod
