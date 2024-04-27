/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/graph_model_addon.hpp"
#include "hesiod/model/nodes.hpp"

#define HSD_NODE_SPACING 300.f

namespace hesiod
{

QtNodes::NodeId add_graph_example(HsdDataFlowGraphModel *p_model,
                                  std::string            node_type,
                                  std::string            category)
{
  if (node_type == "ExpandShrink")
  {
    QtNodes::NodeId node_id1 = p_model->addNode("NoiseFbm");
    QtNodes::NodeId node_id2 = p_model->addNode(QString::fromStdString(node_type));
    QtNodes::NodeId node_id3 = p_model->addNode(QString::fromStdString(node_type));

    p_model->addConnection(QtNodes::ConnectionId(node_id1, 0, node_id2, 0));
    p_model->addConnection(QtNodes::ConnectionId(node_id1, 0, node_id3, 0));

    p_model->setNodeData(node_id1, QtNodes::NodeRole::Position, QPointF(0.f, 0.f));
    p_model->setNodeData(node_id2,
                         QtNodes::NodeRole::Position,
                         QPointF(HSD_NODE_SPACING, 0.f));
    p_model->setNodeData(node_id3,
                         QtNodes::NodeRole::Position,
                         QPointF(HSD_NODE_SPACING, HSD_NODE_SPACING));

    {
      ExpandShrink *p_node = p_model->delegateModel<ExpandShrink>(node_id2);
      p_node->attr.at("radius")->get_ref<FloatAttribute>()->value = 0.25f;
      p_node->compute();
    }

    {
      ExpandShrink *p_node = p_model->delegateModel<ExpandShrink>(node_id3);
      p_node->attr.at("radius")->get_ref<FloatAttribute>()->value = 0.25f;
      p_node->attr.at("shrink")->get_ref<BoolAttribute>()->value = true;
      p_node->compute();
    }

    return node_id2;
  }
  else if (node_type == "Gradient")
  {
    float pos_x = 0.f;
    float pos_y = 0.f;

    QtNodes::NodeId node_id1 = p_model->addNode("NoiseFbm");
    QtNodes::NodeId node_id2 = p_model->addNode("Gradient");
    QtNodes::NodeId node_id3 = p_model->addNode("DataPreview");
    QtNodes::NodeId node_id4 = p_model->addNode("DataPreview");

    p_model->addConnection(QtNodes::ConnectionId(node_id1, 0, node_id2, 0));
    p_model->addConnection(QtNodes::ConnectionId(node_id2, 0, node_id3, 0));
    p_model->addConnection(QtNodes::ConnectionId(node_id2, 1, node_id4, 0));

    p_model->setNodeData(node_id2, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));
    pos_x += HSD_NODE_SPACING;

    p_model->setNodeData(node_id2, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));
    pos_x += HSD_NODE_SPACING;

    p_model->setNodeData(node_id3, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));
    pos_y += HSD_NODE_SPACING;

    p_model->setNodeData(node_id4, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));

    return node_id2;
  }
  //
  else if (node_type == "MakeBinary")
  {
    QtNodes::NodeId node_id1 = p_model->addNode("NoiseFbm");
    QtNodes::NodeId node_id2 = p_model->addNode(QString::fromStdString(node_type));

    p_model->addConnection(QtNodes::ConnectionId(node_id1, 0, node_id2, 0));

    p_model->setNodeData(node_id1, QtNodes::NodeRole::Position, QPointF(0.f, 0.f));
    p_model->setNodeData(node_id2,
                         QtNodes::NodeRole::Position,
                         QPointF(HSD_NODE_SPACING, 0.f));

    MakeBinary *p_node = p_model->delegateModel<MakeBinary>(node_id2);
    p_node->attr.at("threshold")->get_ref<FloatAttribute>()->value = 0.5f;
    p_node->compute();

    return node_id2;
  }
  //
  else if (node_type == "Noise")
  {
    float           pos_x = 0.f;
    float           pos_y = 0.f;
    int             k = 0;
    QtNodes::NodeId node_id;

    for (auto &[noise_key, noise_int] : noise_type_map)
    {
      node_id = p_model->addNode("Noise");

      pos_x = (k % 4) * HSD_NODE_SPACING;
      pos_y = (int)(k / 4.f) * HSD_NODE_SPACING;
      p_model->setNodeData(node_id, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));

      Noise *p_node = p_model->delegateModel<Noise>(node_id);
      p_node->attr.at("noise_type")->get_ref<MapEnumAttribute>()->choice = noise_key;
      p_node->compute();

      k++;

      if (k == 12)
        break;
    }

    return node_id;
  }
  //
  else if (node_type == "NoiseFbm")
  {
    float           pos_x = 0.f;
    float           pos_y = 0.f;
    int             k = 0;
    QtNodes::NodeId node_id;

    for (auto &[noise_key, noise_int] : noise_type_map_fbm)
    {
      node_id = p_model->addNode("NoiseFbm");

      pos_x = (k % 4) * HSD_NODE_SPACING;
      pos_y = (int)(k / 4.f) * HSD_NODE_SPACING;
      p_model->setNodeData(node_id, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));

      NoiseFbm *p_node = p_model->delegateModel<NoiseFbm>(node_id);
      p_node->attr.at("noise_type")->get_ref<MapEnumAttribute>()->choice = noise_key;
      p_node->compute();

      k++;

      if (k == 8)
        break;
    }

    return node_id;
  }
  //
  else if (node_type == "ColorizeCmap" || node_type == "SetAlpha")
  {
    float pos_x = 0.f;
    float pos_y = 0.f;

    QtNodes::NodeId node_id1 = p_model->addNode("NoiseFbm");
    QtNodes::NodeId node_id2 = p_model->addNode("Noise");
    QtNodes::NodeId node_id3 = p_model->addNode("ColorizeCmap");
    QtNodes::NodeId node_id4 = p_model->addNode("SetAlpha");

    p_model->addConnection(QtNodes::ConnectionId(node_id1, 0, node_id3, 0));
    p_model->addConnection(QtNodes::ConnectionId(node_id3, 0, node_id4, 0));
    p_model->addConnection(QtNodes::ConnectionId(node_id2, 0, node_id4, 1));

    p_model->setNodeData(node_id1, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));
    pos_x += HSD_NODE_SPACING;

    p_model->setNodeData(node_id2,
                         QtNodes::NodeRole::Position,
                         QPointF(pos_x, pos_y + HSD_NODE_SPACING));

    p_model->setNodeData(node_id3, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));
    pos_x += HSD_NODE_SPACING;

    p_model->setNodeData(node_id4, QtNodes::NodeRole::Position, QPointF(pos_x, pos_y));

    if (node_type == "SetAlpha")
      return node_id4;
    else
      return node_id3;
  }
  //
  else
  {
    int         pos = category.find("/");
    std::string main_category = category.substr(0, pos);

    if (main_category == "Filter" || main_category == "Mask" || main_category == "Math" ||
        main_category == "Operator")
    {
      QtNodes::NodeId node_id1 = p_model->addNode("NoiseFbm");
      QtNodes::NodeId node_id2 = p_model->addNode(QString::fromStdString(node_type));

      p_model->addConnection(QtNodes::ConnectionId(node_id1, 0, node_id2, 0));

      p_model->setNodeData(node_id1, QtNodes::NodeRole::Position, QPointF(0.f, 0.f));
      p_model->setNodeData(node_id2,
                           QtNodes::NodeRole::Position,
                           QPointF(HSD_NODE_SPACING, 0.f));

      return node_id2;
    }

    else
      return p_model->addNode(QString::fromStdString(node_type));
  }
}

} // namespace hesiod
