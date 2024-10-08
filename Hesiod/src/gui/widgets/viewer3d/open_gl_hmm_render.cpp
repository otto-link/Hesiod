/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

// #include "doubleslider.hpp" // from external/Attributes

#include "hmm/src/base.h" // from external/HighMap
#include "hmm/src/heightmap.h"
#include "hmm/src/triangulator.h"

#include "highmap/export.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"
#include "highmap/shadows.hpp"

#include "hesiod/gui/widgets/open_gl/open_gl_hmm_render.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

OpenGLHmmRender::OpenGLHmmRender(QWidget *parent, ShaderType shader_type)
    : OpenGLRender(parent, shader_type)
{
  LOG->trace("OpenGLHmmRender::OpenGLHmmRender");

  this->use_normal_map = true;

  // this->use_normals = true;
}

void OpenGLHmmRender::set_data(BaseNode          *new_p_node,
                               const std::string &new_port_id_elev,
                               const std::string &new_port_id_color)
{
  LOG->trace("OpenGLHmmRender::set_data, [{}] [{}] [{}]",
             p_node ? "non-nullptr" : "nullptr",
             new_port_id_elev,
             new_port_id_color);

  this->p_node = new_p_node;
  this->port_id_elev = new_port_id_elev;
  this->port_id_color = new_port_id_color;

  if (this->p_node)
  {
    LOG->trace("OpenGLHmmRender::set_data, p_node is NOT nullptr");

    int         port_index_elev = this->p_node->get_port_index(this->port_id_elev);
    std::string data_type_elev = this->p_node->get_data_type(port_index_elev);

    // check that data are indeed available
    if (p_node->get_data_ref(port_index_elev))
    {

      // --- elevation

      if (data_type_elev == typeid(hmap::HeightMap).name())
      {
        LOG->trace("OpenGLRender::set_data, HeightMap data");

        hmap::HeightMap *p_h = this->p_node->get_value_ref<hmap::HeightMap>(
            port_index_elev);
        hmap::Array array = p_h->to_array();
        this->texture_shape = array.shape;

        // --- generate triangle mesh

        // bool  add_base = !this->wireframe_mode;
        // float max_error = 1e-3;
        // generate_mesh_approximation(array,
        //                             this->vertices,
        //                             this->indices,
        //                             max_error,
        //                             add_base);

        // generate the basemesh (NB - shape can be modified while
        // editing the graph when the model configuration is changed by
        // the user)
        if (array.size() != (int)this->vertices.size())
          generate_basemesh(array.shape, this->vertices, this->indices);

        update_vertex_elevations(array, this->vertices);

        // --- normal map texture

        if (this->use_normal_map)
        {
          hmap::Tensor normals3 = hmap::compute_nmap(array);
          this->texture_normal_map = normals3.to_img_8bit();
        }

        // --- diffuse texture

        // default coloring (if no data available or no coloring
        // strategy for the data type)
        if (true)
        {
          this->texture_diffuse.resize(4 * array.shape.x * array.shape.y);

          hmap::Array hs = hillshade(array,
                                     180.f,
                                     45.f,
                                     10.f * array.ptp() / (float)array.shape.x);
          hmap::remap(hs);
          hs = hmap::pow(hs, 1.5f);

          int k = 0;
          for (int j = array.shape.y - 1; j > -1; j--)
            for (int i = 0; i < array.shape.x; i++)
            {
              this->texture_diffuse[k++] = (uint8_t)(255.f * hs(i, j));
              this->texture_diffuse[k++] = (uint8_t)(255.f * hs(i, j));
              this->texture_diffuse[k++] = (uint8_t)(255.f * hs(i, j));
              this->texture_diffuse[k++] = (uint8_t)(255.f);
            }
        }

        // send to OpenGL buffers and update
        this->bind_gl_buffers();
      }
    }
  }

  this->repaint();
}

} // namespace hesiod
