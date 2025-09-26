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

std::filesystem::path ensure_extension(std::filesystem::path fname,
                                       const std::string    &extension)
{
  std::string ext = extension;

  // ensure extension starts with a dot
  if (!ext.empty() && ext.front() != '.')
    ext = "." + ext;

  // if the extension is missing or different, replace it
  if (!fname.has_extension() || fname.extension() != ext)
    fname.replace_extension(ext);

  return fname;
}

std::filesystem::path insert_before_extension(const std::filesystem::path &original_path,
                                              const std::string           &insert_str)
{
  std::filesystem::path dir = original_path.parent_path();
  std::string           stem = original_path.stem().string();
  std::string           ext = original_path.extension().string();
  std::filesystem::path new_name = stem + insert_str + ext;
  return dir / new_name;
}

std::string insert_char_every_nth(const std::string &input,
                                  std::size_t        n,
                                  const std::string &chr)
{
  if (n == 0)
    return input; // avoid division by zero

  std::string result;
  result.reserve(input.size() + input.size() / n * chr.size());

  for (std::size_t i = 0; i < input.size(); ++i)
  {
    result += input[i];
    if ((i + 1) % n == 0)
    {
      result += chr;
    }
  }

  return result;
}

nlohmann::json json_from_file(const std::string &fname)
{
  nlohmann::json json;
  std::ifstream  file(fname);

  if (file.is_open())
  {
    file >> json;
    file.close();
    Logger::log()->trace("json_from_file: JSON successfully loaded from {}", fname);
  }
  else
  {
    Logger::log()->error("json_from_file: Could not open file {} to load JSON", fname);
  }

  return json;
}

void json_to_file(const nlohmann::json &json,
                  const std::string    &fname,
                  bool                  merge_with_existing_content)
{
  nlohmann::json final_json = json;

  if (merge_with_existing_content)
  {
    std::ifstream infile(fname);
    if (infile.is_open())
    {
      try
      {
        nlohmann::json existing;
        infile >> existing;
        infile.close();

        // Merge new JSON into existing JSON
        existing.merge_patch(json);
        final_json = existing;

        Logger::log()->trace("json_to_file: merged JSON with existing content in {}",
                             fname);
      }
      catch (const std::exception &e)
      {
        Logger::log()->warn(
            "json_to_file: Could not parse existing JSON in {} ({}). Overwriting "
            "instead.",
            fname,
            e.what());
      }
    }
  }

  std::ofstream outfile(fname);
  if (outfile.is_open())
  {
    outfile << final_json.dump(4);
    outfile.close();
    Logger::log()->trace("json_to_file: JSON successfully written to {}", fname);
  }
  else
  {
    Logger::log()->error("json_to_file: Could not open file {} to save JSON", fname);
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
unsigned int to_uint_safe(const std::string &str)
{
  try
  {
    size_t        pos;
    unsigned long val = std::stoul(str, &pos);
    if (pos != str.size())
    {
      throw std::invalid_argument("Trailing characters");
    }
    if (val > std::numeric_limits<unsigned int>::max())
    {
      throw std::out_of_range("Value too large for unsigned int");
    }
    return static_cast<unsigned int>(val);
  }
  catch (const std::exception &e)
  {
    Logger::log()->error("Conversion error: {}", e.what());
    return 0; // or throw / handle differently
  }
}

} // namespace hesiod
