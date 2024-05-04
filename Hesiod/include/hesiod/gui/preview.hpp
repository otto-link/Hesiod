/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QLabel>
#include <QWidget>

#include "hesiod/data/heightmap_data.hpp"

namespace hesiod
{

class BaseNode; // forward decl.

/**
 * @brief Preview class, generic node preview widget (can be used for inputs and outputs).
 */
class Preview : public QWidget
{
  Q_OBJECT

public:
  Preview() = default;

  Preview(BaseNode *p_node);

public Q_SLOTS:
  void update_image();

private:
  BaseNode *p_node;
  QLabel   *label;
};

/**
 * @brief PreviewVec2 class, node preview widget for a pair of arrays, like a vector field
 * (can only be used for outputs).
 */
class PreviewVec2 : public QWidget
{
  Q_OBJECT

public:
  PreviewVec2() = default;

  PreviewVec2(const std::shared_ptr<HeightMapData> *p_u,
              const std::shared_ptr<HeightMapData> *p_v,
              const ModelConfig                    *p_config);

public Q_SLOTS:
  void update_image();

private:
  const std::shared_ptr<HeightMapData> *p_u, *p_v;
  const ModelConfig                    *p_config;
  QLabel                               *label;
};

} // namespace hesiod