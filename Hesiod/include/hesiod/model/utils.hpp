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
#include <string>

#include "nlohmann/json.hpp"

namespace hesiod
{

nlohmann::json json_from_file(const std::string &fname);
void           json_to_file(const nlohmann::json &json, const std::string &fname);

std::vector<std::string> split_string(const std::string &string, char delimiter);

std::string time_stamp();

} // namespace hesiod
