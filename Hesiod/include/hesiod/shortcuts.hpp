#pragma once
#include "hesiod/serialization.hpp"
#include <map>

namespace hesiod::shortcuts
{

using ViewShortcutGroupId = std::string;

class ViewShortcut : public hesiod::serialization::SerializationBatchBase
{
public:
  using Delegate = void (*)(void *pass_data);

  ViewShortcut(std::string shortcut_label,
               int         shortcut_key,
               int         shortcut_modifier,
               Delegate    shortcut_delegate,
               ViewShortcutGroupId shortcut_group_id,
               bool        shortcut_enabled = true);
  ~ViewShortcut() = default;

  void pass_and_check(int shortcut_key, int shortcut_modifier, ViewShortcutGroupId focused_group_id, void *pass_data);
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
  ViewShortcutGroupId group_id;
  bool                enabled;
};

class ViewShortcutsManager : public hesiod::serialization::SerializationBase
{
public:
  ViewShortcutsManager();
  ~ViewShortcutsManager();

  bool add_shortcut(ViewShortcut *shortcut);
  bool remove_shortcut(std::string Label);
  bool remove_all_shortcuts();
  void pass_and_check(int shortcut_key, int shortcut_modifier, void *pass_data);
  void set_focused_group_id(ViewShortcutGroupId shortcut_group_id);

  SERIALIZATION_V2_IMPLEMENT_BASE();
private:
  std::map<std::string, ViewShortcut *> shortcuts;
  ViewShortcutGroupId focused_group_id;
};

} // namespace hesiod::shortcuts