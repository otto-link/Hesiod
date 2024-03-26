/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/model/base_node.hpp"
#include "hesiod/model/heightmap_data.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class Noise : public BaseNode
{

public:
  Noise(const ModelConfig &config)
  {
    LOG_DEBUG("NoiseNode::NoiseNode");
    config.log_debug();

    this->node_caption = "Noise";
    this->input_types = {HeightMapData().type(),
                         HeightMapData().type(),
                         HeightMapData().type()};
    this->output_types = {HeightMapData().type()};
    this->input_captions = {"dx", "dy", "envelope"};
    this->output_captions = {"output"};

    this->out = std::make_shared<HeightMapData>(config);

    this->compute();
  }

public:
  QJsonObject save() const override
  {
    QJsonObject modelJson = QtNodes::NodeDelegateModel::save();

    modelJson["kw.x"] = QString::number(this->kw.x);
    modelJson["kw.y"] = QString::number(this->kw.y);
    modelJson["seed"] = QString::number(this->seed);
    return modelJson;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_float(p["kw.x"], this->kw.x);
    ret &= convert_qjsonvalue_to_float(p["kw.y"], this->kw.y);
    ret &= convert_qjsonvalue_to_uint(p["seed"], this->seed);

    if (!ret)
      LOG_ERROR("serialization error when loading node [%s]",
                this->name().toStdString().c_str());
  }

public:
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex /* port_index */) override
  {
    return std::static_pointer_cast<QtNodes::NodeData>(this->out);
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex                 port_index) override
  {
    if (!data)
      Q_EMIT this->dataInvalidated(0);

    switch (port_index)
    {
    case 0:
      this->dx = std::dynamic_pointer_cast<HeightMapData>(data);
      break;
    case 1:
      this->dy = std::dynamic_pointer_cast<HeightMapData>(data);
      break;
    case 2:
      this->envelope = std::dynamic_pointer_cast<HeightMapData>(data);
    }

    this->compute();
  }

  QWidget *embeddedWidget() override { return nullptr; }

protected:
  hmap::Vec2<float> kw = {DEFAULT_KW, DEFAULT_KW};
  uint              seed = DEFAULT_SEED;

private:
    std::weak_ptr<HeightMapData> dx, dy, envelope;
    std::shared_ptr<HeightMapData> out;

  void compute()
  {
    QtNodes::PortIndex const out_port_index = 0;

    LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

    // base noise function
    hmap::HeightMap *p_dx = HSD_GET_POINTER(this->dx);
    hmap::HeightMap *p_dy = HSD_GET_POINTER(this->dy);

    hmap::fill(*this->out->get_ref(),
               p_dx,
               p_dy,
               [this](hmap::Vec2<int>   shape,
                      hmap::Vec4<float> bbox,
                      hmap::Array      *p_noise_x,
                      hmap::Array      *p_noise_y)
               {
                 return hmap::noise(hmap::NoiseType::n_perlin,
                                    shape,
                                    this->kw,
                                    this->seed,
                                    p_noise_x,
                                    p_noise_y,
                                    nullptr,
                                    bbox);
               });

    // add envelope
    hmap::HeightMap *p_env = HSD_GET_POINTER(this->envelope);

    if (p_env)
    {
      float hmin = this->out->get_ref()->min();
      hmap::transform(*this->out->get_ref(),
                      *p_env,
                      [&hmin](hmap::Array &a, hmap::Array &b)
                      {
                        a -= hmin;
                        a *= b;
                      });
    }

    hmap::Array atmp = this->out->get_ref()->to_array();
    atmp.to_png_grayscale_8bit("out.png");

    Q_EMIT this->dataUpdated(out_port_index);
  }
};

} // namespace hesiod