#include "hesiod/shortcuts.hpp"
#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"
#include <map>

namespace hesiod::gui
{

// Shortcut

Shortcut::Shortcut(std::string     shortcut_label,
                   int             shortcut_key,
                   int             shortcut_modifier,
                   ShortcutDelegate        shortcut_delegate,
                   ShortcutGroupId shortcut_group_id,
                   bool            shortcut_enabled)
    : label(shortcut_label), key(shortcut_key), modifier(shortcut_modifier),
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

bool ShortcutsManager::add_shortcut(Shortcut *shortcut)
{
  if (this->shortcuts.count(shortcut->get_label()) > 0)
  {
    return false;
  }

  this->shortcuts.emplace(shortcut->get_label(), shortcut);
  return true;
}

bool ShortcutsManager::remove_shortcut(std::string label)
{
  if (this->shortcuts.count(label) <= 0)
  {
    return false;
  }

  Shortcut *s = this->shortcuts.at(label);
  this->shortcuts.erase(label);

  delete s;
  return true;
}

bool ShortcutsManager::remove_all_shortcuts()
{
  bool res = true;

  for (std::map<std::string, Shortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    res &= this->remove_shortcut(currentIterator->first);
  }

  return res;
}

void ShortcutsManager::pass_and_check(int shortcut_key, int shortcut_modifier)
{
  if (input_blocked == true)
    return;

  for (std::map<std::string, Shortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    currentIterator->second->pass_and_check(shortcut_key,
                                            shortcut_modifier,
                                            focused_group_id);
  }
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
  for (std::map<std::string, Shortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    nlohmann::json currentData = nlohmann::json();
    currentIterator->second->serialize_json_v2("data", currentData);
    output_data[field_name][currentIterator->first] = currentData;
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

  for (auto currentIterator : input_data[field_name].items())
  {
    std::string label = currentIterator.key();

    if (currentIterator.value().is_object() == false)
    {
      LOG_ERROR("Current shortcut data is not an object!");
      continue;
    }

    if (this->shortcuts.count(label) <= 0)
    {
      LOG_ERROR("Short with the label of %s could not be found!", label.data());
      continue;
    }

    this->shortcuts.at(label)->deserialize_json_v2("data",
                                                   currentIterator.value());
  }

  return true;
}

} // namespace hesiod::gui
