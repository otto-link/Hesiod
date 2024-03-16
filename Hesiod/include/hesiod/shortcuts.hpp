#pragma once
#include "hesiod/serialization.hpp"
#include <functional>
#include <map>

namespace hesiod::gui
{

using ShortcutGroupId = std::string;

class ShortcutGroupElement
{
public:
  virtual ~ShortcutGroupElement() = default;

  virtual ShortcutGroupId get_element_shortcut_group_id()
  {
    return "Default Group";
  }
};

class Shortcut : public hesiod::serialization::SerializationBatchBase
{
public:
  using ShortcutDelegate = std::function<void()>;

  Shortcut(std::string      shortcut_label,
           int              shortcut_key,
           int              shortcut_modifier,
           ShortcutDelegate shortcut_delegate,
           ShortcutGroupId  shortcut_group_id,
           bool             shortcut_enabled = true);

  ~Shortcut() = default;

  void pass_and_check(int             shortcut_key,
                      int             shortcut_modifier,
                      ShortcutGroupId focused_group_id);

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

  ShortcutDelegate get_delegate()
  {
    return delegate;
  }

private:
  std::string      label;
  int              key;
  int              modifier;
  ShortcutDelegate delegate;
  ShortcutGroupId  group_id;
  bool             enabled;
};

class ShortcutsManager : public hesiod::serialization::SerializationBase
{
public:
  ShortcutsManager();
  ~ShortcutsManager();

  bool add_shortcut(Shortcut *shortcut);
  bool remove_shortcut(std::string Label);
  bool remove_all_shortcuts();
  void pass_and_check(int shortcut_key, int shortcut_modifier);
  void set_focused_group_id(ShortcutGroupId shortcut_group_id);
  void set_input_blocked(bool toggle); // Will be reset after each frame!

  SERIALIZATION_V2_IMPLEMENT_BASE();

private:
  std::map<std::string, Shortcut *> shortcuts;
  ShortcutGroupId                   focused_group_id;
  bool                              input_blocked;
};

} // namespace hesiod::gui