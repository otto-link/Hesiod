#include "hesiod/shortcuts.hpp"

namespace hesiod::shortcuts
{

// ViewShortcut

ViewShortcut::ViewShortcut(std::string shortcut_label,
                           int         shortcut_key,
                           int         shortcut_modifier,
                           Delegate    shortcut_delegate,
                           bool        shortcut_enabled)
    : label(shortcut_label), key(shortcut_key), modifier(shortcut_modifier),
      delegate(shortcut_delegate), enabled(shortcut_enabled)
{
}

void ViewShortcut::PassAndCheck(int   shortcut_key,
                                int   shortcut_modifier,
                                void *pass_data)
{
  if (key != shortcut_key || modifier != shortcut_modifier || enabled != true)
  {
    return;
  }

  delegate(pass_data);
}

serialization::SerializationBatchHelper ViewShortcut::BuildBatchHelperData()
{
  serialization::SerializationBatchHelper batch =
      serialization::SerializationBatchHelper();

  batch.AddInt("key", &key);
  batch.AddInt("modifier", &modifier);
  batch.AddBool("enabled", &enabled);

  return batch;
}

// ViewShortcutsManager

ViewShortcutsManager::ViewShortcutsManager() : shortcuts()
{
}

ViewShortcutsManager::~ViewShortcutsManager()
{
  RemoveAllShortcuts();
}

bool ViewShortcutsManager::AddShortcut(ViewShortcut *shortcut)
{
  if (shortcuts.count(shortcut->GetLabel()) > 0)
  {
    return false;
  }

  shortcuts.emplace(shortcut->GetLabel(), shortcut);
  return true;
}

bool ViewShortcutsManager::RemoveShortcut(std::string Label)
{
  if (shortcuts.count(Label) <= 0)
  {
    return false;
  }

  ViewShortcut *s = shortcuts.at(Label);
  shortcuts.erase(Label);

  delete s;
  return true;
}

bool ViewShortcutsManager::RemoveAllShortcuts()
{
  bool res = true;

  for (std::map<std::string, ViewShortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    res &= RemoveShortcut(currentIterator->first);
  }

  return res;
}

void ViewShortcutsManager::PassAndCheck(int   shortcut_key,
                                        int   shortcut_modifier,
                                        void *pass_data)
{
  for (std::map<std::string, ViewShortcut *>::iterator currentIterator =
           shortcuts.begin();
       currentIterator != shortcuts.end();
       currentIterator++)
  {
    currentIterator->second->PassAndCheck(shortcut_key,
                                          shortcut_modifier,
                                          pass_data);
  }
}
} // namespace hesiod::shortcuts