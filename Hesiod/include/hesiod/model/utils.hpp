/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file utils.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <algorithm>
#include <filesystem>
#include <string>

#include "nlohmann/json.hpp"

namespace hesiod
{

template <typename T> bool contains(const std::vector<T> &vec, const T &item)
{
  return std::find(vec.begin(), vec.end(), item) != vec.end();
}

std::filesystem::path ensure_extension(std::filesystem::path fname,
                                       const std::string    &extension);

std::filesystem::path insert_before_extension(const std::filesystem::path &original_path,
                                              const std::string           &insert_str);

nlohmann::json json_from_file(const std::string &fname);
void           json_to_file(const nlohmann::json &json, const std::string &fname);

std::string              insert_char_every_nth(const std::string &input,
                                               std::size_t        n,
                                               const std::string &chr);
std::vector<std::string> split_string(const std::string &string, char delimiter);

std::string time_stamp();

} // namespace hesiod
