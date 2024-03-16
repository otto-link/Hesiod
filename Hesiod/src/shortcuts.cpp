#include "hesiod/shortcuts.hpp"
#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"
#include <map>

namespace hesiod::gui
{

// GuiShortcut

GuiShortcut::GuiShortcut(std::string        shortcut_label,
                         int                shortcut_key,
                         int                shortcut_modifier,
                         Delegate           shortcut_delegate,
                         GuiShortcutGroupId shortcut_group_id,
                         bool               shortcut_enabled)
    : label(shortcut_label), key(shortcut_key), modifier(shortcut_modifier),
      delegate(shortcut_delegate), group_id(shortcut_group_id),
      enabled(shortcut_enabled)
{
}

void GuiShortcut::pass_and_check(int                shortcut_key,
                                 int                shortcut_modifier,
                                 GuiShortcutGroupId focused_group_id,
                                 void              *pass_data)
{
  if (key != shortcut_key || modifier != shortcut_modifier ||
      group_id != focused_group_id || enabled != true)
  {
    return;
  }

  delegate(pass_data);
}

serialization::SerializationBatchHelper GuiShortcut::BuildBatchHelperData()
{
  serialization::SerializationBatchHelper batch =
      serialization::SerializationBatchHelper();

  batch.AddInt("key", &key);
  batch.AddInt("modifier", &modifier);
  batch.AddBool("enabled", &enabled);

  return batch;
}

// GuiShortcutsManager

GuiShortcutsManager::GuiShortcutsManager() : shortcuts(), focused_group_id()
{
}

GuiShortcutsManager::~GuiShortcutsManager()
{
  this->remove_all_shortcuts();
}

bool GuiShortcutsManager::add_shortcut(GuiShortcut *shortcut)
{
  if (shortcuts.count(shortcut->get_label()) > 0)
  {
    return false;
  }

  shortcuts.emplace(shortcut->get_label(), shortcut);
  return true;
}

bool GuiShortcutsManager::remove_shortcut(std::string Label)
{
  if (shortcuts.count(Label) <= 0)
  {
    return false;
  }

  GuiShortcut *s = shortcuts.at(Label);
  shortcuts.erase(Label);

  delete s;
  return true;
}

bool GuiShortcutsManager::remove_all_shortcuts()
{
  bool res = true;

  for (std::map<std::string, GuiShortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    res &= remove_shortcut(currentIterator->first);
  }

  return res;
}

void GuiShortcutsManager::pass_and_check(int   shortcut_key,
                                         int   shortcut_modifier,
                                         void *pass_data)
{
  if (input_blocked == true)
    return;

  for (std::map<std::string, GuiShortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    currentIterator->second->pass_and_check(shortcut_key,
                                            shortcut_modifier,
                                            focused_group_id,
                                            pass_data);
  }
}

void GuiShortcutsManager::set_focused_group_id(
    GuiShortcutGroupId shortcut_group_id)
{
  this->focused_group_id = shortcut_group_id;
}

void GuiShortcutsManager::set_input_blocked(bool toggle)
{
  this->input_blocked = toggle;
}

bool GuiShortcutsManager::serialize_json_v2(std::string     field_name,
                                            nlohmann::json &output_data)
{
  for (std::map<std::string, GuiShortcut *>::iterator currentIterator =
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

bool GuiShortcutsManager::deserialize_json_v2(std::string     field_name,
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