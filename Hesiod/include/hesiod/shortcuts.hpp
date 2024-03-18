/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <functional>
#include <map>
#include <memory>

#include "hesiod/serialization.hpp"

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

  Shortcut(std::string      shortcut_id,
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

  std::string get_id()
  {
    return id;
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
  std::string      id;
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

  bool add_shortcut(std::unique_ptr<Shortcut> p_shortcut);
  bool remove_shortcut(std::string id);
  bool remove_all_shortcuts();
  void pass_and_check(int shortcut_key, int shortcut_modifier);
  void set_focused_group_id(ShortcutGroupId shortcut_group_id);
  void set_input_blocked(bool toggle); // Will be reset after each frame!

  Shortcut *get_shortcut_ref_by_id(std::string id)
  {
    return this->shortcuts.at(id).get();
  }

  SERIALIZATION_V2_IMPLEMENT_BASE();

private:
  std::map<std::string, std::unique_ptr<Shortcut>> shortcuts;
  ShortcutGroupId                                  focused_group_id;
  bool                                             input_blocked;
};

} // namespace hesiod::gui
