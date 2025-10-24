/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <functional>

#include <QApplication>
#include <QGridLayout>
#include <QLayout>
#include <QWidget>

namespace hesiod
{

void add_qmenu_spacer(QMenu *menu, int height = 8);

void apply_global_style(QApplication &app);

/**
 * @brief Clear all items from a given layout.
 *
 * This function removes and deletes all widgets contained within the specified layout,
 * effectively clearing it of any child widgets.
 *
 * @param layout Reference to the layout that will be cleared.
 */
void clear_layout(QLayout *layout);

int get_column_count(QGridLayout *layout);
int get_row_count(QGridLayout *layout);

/**
 * @brief Resize the font of a widget.
 *
 * Adjust the font size of the specified widget by a relative size modification. This
 * allows dynamic resizing of widget fonts for responsive design or user preferences.
 *
 * @param widget                     Reference to the widget whose font size is to be
 *                                   adjusted.
 * @param relative_size_modification The relative modification to the current font size
 *                                   (e.g., +1 to increase font size by 1 point, -1 to
 *                                   decrease it).
 */
void resize_font(QWidget *widget, int relative_size_modification);

void render_widget_screenshot(QWidget              *widget,
                              const std::string    &fname,
                              const QSize          &size = QSize(),
                              std::function<void()> post_render_callback = nullptr);

int   float_to_slider_pos(float v, float min, float max, int slider_steps);
float slider_pos_to_float(int pos, float min, float max, int slider_steps);

} // namespace hesiod
