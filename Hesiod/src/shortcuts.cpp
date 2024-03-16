/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <map>

#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"

#include "hesiod/shortcuts.hpp"

namespace hesiod::gui
{

// Shortcut

Shortcut::Shortcut(std::string      shortcut_id,
                   int              shortcut_key,
                   int              shortcut_modifier,
                   ShortcutDelegate shortcut_delegate,
                   ShortcutGroupId  shortcut_group_id,
                   bool             shortcut_enabled)
    : id(shortcut_id), key(shortcut_key), modifier(shortcut_modifier),
      delegate(shortcut_delegate), group_id(shortcut_group_id),
      enabled(shortcut_enabled)
{
}

void Shortcut::pass_and_check(int             shortcut_key,
                              int             shortcut_modifier,
                              ShortcutGroupId focused_group_id)
{
  if (this->key != shortcut_key || this->modifier != shortcut_modifier ||
      this->group_id != focused_group_id || this->enabled != true)
  {
    return;
  }

  this->delegate();
}

serialization::SerializationBatchHelper Shortcut::BuildBatchHelperData()
{
  serialization::SerializationBatchHelper batch =
      serialization::SerializationBatchHelper();

  batch.AddInt("key", &this->key);
  batch.AddInt("modifier", &this->modifier);
  batch.AddBool("enabled", &this->enabled);

  return batch;
}

// ShortcutsManager

ShortcutsManager::ShortcutsManager() : shortcuts(), focused_group_id()
{
}

ShortcutsManager::~ShortcutsManager()
{
  this->remove_all_shortcuts();
}

bool ShortcutsManager::add_shortcut(std::unique_ptr<Shortcut> p_shortcut)
{
  if (this->shortcuts.contains(p_shortcut->get_id()))
  {
    LOG_ERROR("shortcut id [%s] already used", p_shortcut->get_id().c_str());
    return false;
  }
  else
  {
    this->shortcuts.emplace(p_shortcut->get_id(), std::move(p_shortcut));
    return true;
  }
}

bool ShortcutsManager::remove_shortcut(std::string id)
{
  if (this->shortcuts.contains(id))
  {
    this->shortcuts.erase(id);
    return true;
  }
  else
  {
    LOG_ERROR("shortcut id [%s] unknown", id.c_str());
    return false;
  }
}

bool ShortcutsManager::remove_all_shortcuts()
{
  this->shortcuts.clear();
  return true;
}

void ShortcutsManager::pass_and_check(int shortcut_key, int shortcut_modifier)
{
  if (input_blocked == true)
    return;

  for (auto &[id, p_shortcut] : this->shortcuts)
    p_shortcut->pass_and_check(shortcut_key,
                               shortcut_modifier,
                               focused_group_id);
}

void ShortcutsManager::set_focused_group_id(ShortcutGroupId shortcut_group_id)
{
  this->focused_group_id = shortcut_group_id;
}

void ShortcutsManager::set_input_blocked(bool toggle)
{
  this->input_blocked = toggle;
}

bool ShortcutsManager::serialize_json_v2(std::string     field_name,
                                         nlohmann::json &output_data)
{
  for (auto &[id, p_shortcut] : this->shortcuts)
  {
    nlohmann::json data = nlohmann::json();
    p_shortcut->serialize_json_v2("data", data);
    output_data[field_name][id] = data;
  }

  return true;
}

bool ShortcutsManager::deserialize_json_v2(std::string     field_name,
                                           nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false)
  {
    return false;
  }

  for (auto data : input_data[field_name].items())
  {
    std::string id = data.key();

    if (data.value().is_object() == false)
    {
      LOG_ERROR("Current shortcut data is not an object!");
      continue;
    }

    if (this->shortcuts.count(id) <= 0)
    {
      LOG_ERROR("Short with the id of %s could not be found!", id.data());
      continue;
    }

    this->shortcuts.at(id)->deserialize_json_v2("data", data.value());
  }

  return true;
}

} // namespace hesiod::gui
