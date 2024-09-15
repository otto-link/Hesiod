/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file base_node.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <QObject>

#include "nlohmann/json.hpp"

#include "gnode/node.hpp"

#include "hesiod/model/attributes/attributes.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class BaseNode : public QObject, public gnode::Node
{
  Q_OBJECT

public:
  BaseNode() = default;

  BaseNode(const std::string &label, std::shared_ptr<ModelConfig> config);

  template <typename AttributeType>
  auto get_attr(const std::string &key) -> decltype(auto)
  {
    return this->attr.at(key)->get_ref<AttributeType>()->get();
  }

  std::string get_category() const { return this->category; }

  nlohmann::json get_documentation() const { return this->documentation; }

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

Q_SIGNALS:
  void compute_started(std::string id);

  void compute_finished(std::string id);

  void node_deserialized(std::string id);

protected:
  std::map<std::string, std::unique_ptr<Attribute>> attr = {};

  /**
   * @brief Defines a preferred display order of the attributes.
   */
  std::vector<std::string> attr_ordered_key = {};

  std::string category;

private:
  std::shared_ptr<ModelConfig> config;

  nlohmann::json documentation;
};

} // namespace hesiod