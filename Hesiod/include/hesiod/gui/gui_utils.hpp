/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QLayout>

namespace hesiod
{

/**
 * @brief Clear a given layout.
 * @param layout Reference to the layout.
 */
void clear_layout(QLayout *layout);

/**
 * @brief Resize the widget font
 * @param widget Reference to the widget.
 * @param relative_size_modification Relative size modification (+/- with respect to the
 * current font size).
 */
void resize_font(QWidget *widget, int relative_size_modification);

} // namespace hesiod