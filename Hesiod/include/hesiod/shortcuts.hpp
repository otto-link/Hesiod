#pragma once
#include "hesiod/serialization.hpp"
#include <map>

namespace hesiod::shortcuts
{

    class ViewShortcut : public hesiod::serialization::SerializationBatchBase
    {
    public:
        using Delegate = void(*)(void* pass_data);
        
        ViewShortcut(std::string shortcut_label, int shortcut_key, int shortcut_modifier, Delegate shortcut_delegate, bool shortcut_enabled = true);
        ~ViewShortcut() = default;

        void PassAndCheck(int shortcut_key, int shortcut_modifier, void* pass_data);
        SERIALIZATION_V2_IMPLEMENT_BATCH_BASE();

        std::string GetLabel() { return label; }
        int GetKey() { return key; }
        int GetModifier() { return modifier; }
        Delegate GetDelegate() { return delegate; }        
    private:
        std::string label;
        int key;
        int modifier;
        Delegate delegate;
        bool enabled;
    };

    class ViewShortcutsManager : public hesiod::serialization::SerializationBase
    {
    public:
        ViewShortcutsManager();
        ~ViewShortcutsManager();

        bool AddShortcut(ViewShortcut* shortcut);
        bool RemoveShortcut(std::string Label);
        bool RemoveAllShortcuts();
        void PassAndCheck(int shortcut_key, int shortcut_modifier, void* pass_data);
    private:
        std::map<std::string, ViewShortcut*> shortcuts;
    };

}