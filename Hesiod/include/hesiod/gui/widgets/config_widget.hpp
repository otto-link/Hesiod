/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file config_widget.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QWidget>

#include "hesiod/config.hpp"

namespace hesiod
{

class ConfigWidget : public QWidget
{
  Q_OBJECT

public:
  ConfigWidget(QWidget *parent = nullptr);

private:
  bool flat_layout = true;
};

} // namespace hesiod