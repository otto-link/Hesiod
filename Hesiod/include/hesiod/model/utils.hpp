/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_set>

#include <QColor>

#include "hesiod/logger.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace hesiod
{

// --- Vectors

template <typename T> bool contains(const std::vector<T> &vec, const T &item)
{
  return std::find(vec.begin(), vec.end(), item) != vec.end();
}

template <typename T>
std::vector<T> merge_unique(const std::vector<T> &a, const std::vector<T> &b)
{
  std::unordered_set<T> seen;
  std::vector<T>        result;

  for (const auto &item : a)
  {
    if (seen.insert(item).second)
      result.push_back(item);
  }

  for (const auto &item : b)
  {
    if (seen.insert(item).second)
      result.push_back(item);
  }

  return result;
}

template <typename T> void remove_all_occurrences(std::vector<T> &vec, const T &value)
{
  vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

// --- Filenames

std::filesystem::path ensure_extension(std::filesystem::path fname,
                                       const std::string    &extension);

std::filesystem::path insert_before_extension(const std::filesystem::path &original_path,
                                              const std::string           &insert_str);

// --- json

nlohmann::json json_from_file(const std::string &fname);
void           json_to_file(const nlohmann::json &json,
                            const std::string    &fname,
                            bool                  merge_with_existing_content = false);

template <typename T>
inline void json_safe_get(const nlohmann::json &j, const std::string &key, T &value)
{
  if (j.contains(key))
  {
    value = j.at(key).get<T>();
  }
  else
  {
    Logger::log()->error("Required json key \"{}\" not found.", key);
  }
}

inline void json_safe_get(const nlohmann::json &j, const std::string &key, QColor &value)
{
  if (j.contains(key))
  {
    QString str = j.at(key).get<std::string>().c_str();
    if (QColor::isValidColor(str))
      value = QColor(str);
    else
      Logger::log()->error("Invalid QColor for key \"{}\"", key);
  }
  else
  {
    Logger::log()->error("Required json key \"{}\" not found.", key);
  }
}

// --- Strings

std::string insert_char_every_nth(const std::string &input,
                                  std::size_t        n,
                                  const std::string &chr);

void replace_all(std::string &str, const std::string &from, const std::string &to);
std::vector<std::string> split_string(const std::string &string, char delimiter);
std::string              time_stamp();
unsigned int             to_uint_safe(const std::string &str);

} // namespace hesiod
