/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file attribute.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <memory>

#include "nlohmann/json.hpp"

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"

#include "hesiod/logger.hpp"

// clang-format off
#define NEW_ATTR_CLOUD(...) std::make_unique<hesiod::CloudAttribute>(__VA_ARGS__)
#define NEW_ATTR_PATH(...) std::make_unique<hesiod::PathAttribute>(__VA_ARGS__)

#define GET_ATTR_CLOUD(s) this->attr.at(s)->get_ref<CloudAttribute>()->get()
#define GET_ATTR_PATH(s) this->attr.at(s)->get_ref<PathAttribute>()->get()
// clang-format on

namespace hesiod
{

class CloudAttribute : public Attribute
{
public:
  CloudAttribute() = default;

  hmap::Cloud get();

  AttributeType get_type() { return AttributeType::CLOUD; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  hmap::Cloud value;
};

class PathAttribute : public Attribute
{
public:
  PathAttribute() = default;
  hmap::Path    get();
  AttributeType get_type() { return AttributeType::PATH; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  hmap::Path value;
};

} // namespace hesiod
