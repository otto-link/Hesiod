/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file viewer3d.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <QWidget>

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

// =====================================
// ModelConfigWidget
// =====================================
class ModelConfigWidget : public QDialog
{
  Q_OBJECT

public:
  ModelConfigWidget() = default;
  ModelConfigWidget(ModelConfig *p_model_config,
                    bool         show_opencl_config = true,
                    QWidget     *parent = nullptr);

private:
  ModelConfig *p_model_config;

  QSlider *slider_shape;
  QLabel  *label_shape;

  QSlider *slider_tiling;
  QLabel  *label_tiling;

  QSlider *slider_overlap;
  QLabel  *label_overlap;
  float    vmin = 0.f;
  float    vmax = 0.75f;
  int      steps = 3;
};

} // namespace hesiod