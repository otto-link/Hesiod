#pragma once
#include "hesiod/serialization.hpp"
#include <map>

namespace hesiod::shortcuts
{

using GuiShortcutGroupId = std::string;

class GuiShortcut : public hesiod::serialization::SerializationBatchBase
{
public:
  using Delegate = void (*)(void *pass_data);

  GuiShortcut(std::string shortcut_label,
               int         shortcut_key,
               int         shortcut_modifier,
               Delegate    shortcut_delegate,
               GuiShortcutGroupId shortcut_group_id,
               bool        shortcut_enabled = true);
  ~GuiShortcut() = default;

  void pass_and_check(int shortcut_key, int shortcut_modifier, GuiShortcutGroupId focused_group_id, void *pass_data);
  SERIALIZATION_V2_IMPLEMENT_BATCH_BASE();

  std::string get_label()
  {
    return label;
  }
  int get_key()
  {
    return key;
  }
  int get_modifier()
  {
    return modifier;
  }
  Delegate get_delegate()
  {
    return delegate;
  }
private:
  std::string         label;
  int                 key;
  int                 modifier;
  Delegate            delegate;
  GuiShortcutGroupId group_id;
  bool                enabled;
};

class GuiShortcutsManager : public hesiod::serialization::SerializationBase
{
public:
  GuiShortcutsManager();
  ~GuiShortcutsManager();

  bool add_shortcut(GuiShortcut *shortcut);
  bool remove_shortcut(std::string Label);
  bool remove_all_shortcuts();
  void pass_and_check(int shortcut_key, int shortcut_modifier, void *pass_data);
  void set_focused_group_id(GuiShortcutGroupId shortcut_group_id);

  SERIALIZATION_V2_IMPLEMENT_BASE();
private:
  std::map<std::string, GuiShortcut *> shortcuts;
  GuiShortcutGroupId focused_group_id;
};

} // namespace hesiod::shortcuts