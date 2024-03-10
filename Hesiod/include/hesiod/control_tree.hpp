/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"
#include "highmap.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/serialization.hpp"

namespace hesiod::cnode
{

class ControlTree : public gnode::Tree, public serialization::SerializationBase
{
public:
  ControlTree(std::string id);

  ControlTree(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  std::string get_new_id();

  std::string get_node_type(std::string node_id) const;

  inline hmap::Vec2<int> get_shape() const
  {
    return this->shape;
  }

  inline hmap::Vec2<int> get_tiling() const
  {
    return this->tiling;
  }

  inline float get_overlap() const
  {
    return this->overlap;
  }

  void set_serialization_type(
      hesiod::serialization::SerializationType new_serialization_type)
  {
    this->serialization_type = new_serialization_type;
  }

  std::string add_control_node(std::string control_node_type,
                               std::string node_id);

  virtual void clear();

  // serialization (only "load" for this object, used for the batch mode)

  void load_state(std::string     fname,
                  hmap::Vec2<int> new_shape,
                  hmap::Vec2<int> new_tiling,
                  float           new_overlap);

  // specific version with shape, tiling, overlap as inputs
  bool deserialize_json_v2_sto(std::string     field_name,
                               nlohmann::json &input_data,
                               hmap::Vec2<int> new_shape,
                               hmap::Vec2<int> new_tiling,
                               float           new_overlap);

protected:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;

  int id_counter = 0;

  std::string                              json_filename = "";
  hesiod::serialization::SerializationType serialization_type;
};

// --- HELPERS

} // namespace hesiod::cnode
