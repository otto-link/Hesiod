/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file gui_utils.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines utility functions for handling GUI elements, such as layout clearing
 *        and font resizing for widgets in the Hesiod framework.
 *
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023 Otto Link
 * Distributed under the terms of the GNU General Public License.
 */

#pragma once
#include <QLayout>
#include <QWidget>

namespace hesiod
{

class DocumentationPopup : public QWidget
{
public:
  DocumentationPopup(const std::string &title,
                     const std::string &html_source,
                     QWidget           *parent = nullptr);
};

/**
 * @brief Clear all items from a given layout.
 *
 * This function removes and deletes all widgets contained within the specified
 * layout, effectively clearing it of any child widgets.
 *
 * @param layout Reference to the layout that will be cleared.
 */
void clear_layout(QLayout *layout);

/**
 * @brief Resize the font of a widget.
 *
 * Adjust the font size of the specified widget by a relative size modification.
 * This allows dynamic resizing of widget fonts for responsive design or user preferences.
 *
 * @param widget Reference to the widget whose font size is to be adjusted.
 * @param relative_size_modification The relative modification to the current font size
 *        (e.g., +1 to increase font size by 1 point, -1 to decrease it).
 */
void resize_font(QWidget *widget, int relative_size_modification);

} // namespace hesiod
