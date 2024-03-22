#pragma once
#include <span>
#include <vector>


#include "imgui.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

namespace hesiod::gui
{

struct HmapBrushEditorState
{
  bool              is_drawing;
  bool              is_initted;
  float             max_height;
  float             brush_radius;
  float             brush_strength;
  int               blur_strength;
  float             k_clamp_smoothing;
  hmap::Vec2<float> last_mouse_pos;
  ImVec2            canvas_size;
  hmap::Vec2<int>   draw_shape;

  hmap::HeightMap pending_hm;
  std::vector<std::pair<hmap::Vec2<float>, float>>
         pending_changes; // (pos, weight)
  GLuint draw_texture = 0;

  HmapBrushEditorState()
      : is_drawing(false), is_initted(false), max_height(1.f),
        brush_radius(50.f), brush_strength(0.2f), blur_strength(8),
        k_clamp_smoothing(0.5f),
        last_mouse_pos(std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::max()),
        draw_shape({256, 256})
  {
    glGenTextures(1, &this->draw_texture);
  }

  ~HmapBrushEditorState()
  {
    glDeleteTextures(1, &this->draw_texture);
  }

  void add_change(hmap::Vec2<float> pos, float weight);

  void apply_pending_changes();

  void apply_brushes(hmap::HeightMap             &h,
                     std::span<hmap::Array>       kernels,
                     std::span<hmap::Vec2<float>> positions);
};

void hmap_brush_editor(HmapBrushEditorState &state,
                       ImTextureID           canvas_texture,
                       float                 width = 0.0f);

} // namespace hesiod::gui
