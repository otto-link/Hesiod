/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/synthesis.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_texture_quilting_shuffle_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture (guide)");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "heightmap (guide)");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "heightmap", CONFIG);

  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture A");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture B");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture C");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture D");

  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture A out", CONFIG);
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture B out", CONFIG);
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture C out", CONFIG);
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "texture D out", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "patch_width", 0.3f, 0.1f, 1.f);
  ADD_ATTR(FloatAttribute, "overlap", 0.9f, 0.05f, 0.95f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(BoolAttribute, "patch_flip", true);
  ADD_ATTR(BoolAttribute, "patch_rotate", true);
  ADD_ATTR(BoolAttribute, "patch_transpose", true);
  ADD_ATTR(FloatAttribute, "filter_width_ratio", 0.5f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"patch_width",
                                "overlap",
                                "seed",
                                "patch_flip",
                                "patch_rotate",
                                "patch_transpose",
                                "filter_width_ratio"});
}

void compute_texture_quilting_shuffle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightmapRGBA *p_texture_guide = p_node->get_value_ref<hmap::HeightmapRGBA>(
      "texture (guide)");
  hmap::Heightmap *p_hmap_guide = p_node->get_value_ref<hmap::Heightmap>(
      "heightmap (guide)");

  if (p_texture_guide)
  {

    hmap::Heightmap     *p_hmap_out = p_node->get_value_ref<hmap::Heightmap>("heightmap");
    hmap::HeightmapRGBA *p_texture_out = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture");

    hmap::HeightmapRGBA *p_tex_a = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture A");
    hmap::HeightmapRGBA *p_tex_b = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture B");
    hmap::HeightmapRGBA *p_tex_c = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture C");
    hmap::HeightmapRGBA *p_tex_d = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture D");

    hmap::HeightmapRGBA *p_tex_out_a = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture A out");
    hmap::HeightmapRGBA *p_tex_out_b = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture B out");
    hmap::HeightmapRGBA *p_tex_out_c = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture C out");
    hmap::HeightmapRGBA *p_tex_out_d = p_node->get_value_ref<hmap::HeightmapRGBA>(
        "texture D out");

    // for the guide array used to defined the quilting parameters,
    // use the heightmap if available. If not, use the texture luminance
    hmap::Array guide_array;

    if (p_hmap_guide)
      guide_array = p_hmap_guide->to_array();
    else
    {
      hmap::Heightmap luminance = p_texture_guide->luminance();
      guide_array = luminance.to_array();
    }

    // --- define secondary arrays

    std::vector<hmap::Array>   secondary_arrays_storage = {};
    std::vector<hmap::Array *> secondary_arrays_ptr = {};

    // add first each RGBA components that are quilted based on the
    // luminance and then add other secondary textures (also modified
    // based on the guide texture luminance)
    for (auto ptr : {p_texture_guide, p_tex_a, p_tex_b, p_tex_c, p_tex_d})
      if (ptr)
      {
        secondary_arrays_storage.push_back(ptr->rgba[0].to_array());
        secondary_arrays_storage.push_back(ptr->rgba[1].to_array());
        secondary_arrays_storage.push_back(ptr->rgba[2].to_array());
        secondary_arrays_storage.push_back(ptr->rgba[3].to_array());
      }

    // create vector of reference ptrs
    for (auto &v : secondary_arrays_storage)
      secondary_arrays_ptr.push_back(&v);

    // hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("heightmap");

    int ir = std::max(1, (int)(GET("patch_width", FloatAttribute) * p_hmap_out->shape.x));
    hmap::Vec2<int> patch_base_shape = hmap::Vec2<int>(ir, ir);

    // --- work on a single array (i.e. not-tiled algo)

    hmap::Array out_array = hmap::quilting_shuffle(
        guide_array,
        patch_base_shape,
        GET("overlap", FloatAttribute),
        GET("seed", SeedAttribute),
        secondary_arrays_ptr,
        GET("patch_flip", BoolAttribute),
        GET("patch_rotate", BoolAttribute),
        GET("patch_transpose", BoolAttribute),
        GET("filter_width_ratio", FloatAttribute));

    // rebuild outputs
    p_hmap_out->from_array_interp_nearest(out_array);

    // textures
    std::vector<hmap::HeightmapRGBA *> pt_in_vec = {p_texture_guide,
                                                    p_tex_a,
                                                    p_tex_b,
                                                    p_tex_c,
                                                    p_tex_d};
    std::vector<hmap::HeightmapRGBA *> pt_out_vec = {p_texture_out,
                                                     p_tex_out_a,
                                                     p_tex_out_b,
                                                     p_tex_out_c,
                                                     p_tex_out_d};

    // storage index within the secondary_arrays_ptr vector
    int current_idx = 0;

    for (size_t k = 0; k < pt_in_vec.size(); k++)
      if (pt_in_vec[k])
      {
        // rebuild the texture from Heightmap
        hmap::Heightmap r(CONFIG), g(CONFIG), b(CONFIG), a(CONFIG);

        r.from_array_interp_nearest(*secondary_arrays_ptr[current_idx]);
        g.from_array_interp_nearest(*secondary_arrays_ptr[current_idx + 1]);
        b.from_array_interp_nearest(*secondary_arrays_ptr[current_idx + 2]);
        a.from_array_interp_nearest(*secondary_arrays_ptr[current_idx + 3]);

        current_idx += 4;

        *pt_out_vec[k] = hmap::HeightmapRGBA(r, g, b, a);
      }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
