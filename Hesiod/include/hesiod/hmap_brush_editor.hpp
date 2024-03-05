#pragma once
#include <span>
#include "imgui.h"

namespace hesiod::gui
{

struct HmBrushEditorState {
  bool is_drawing;
  bool is_initted;
  float max_height;
  float brush_radius;
  float brush_strength;
  int blur_strength;
  hmap::Vec2<float> last_mouse_pos;
  ImVec2 canvas_size;

  hmap::HeightMap pending_hm;
  std::vector<std::pair<hmap::Vec2<float>, float>> pending_changes; // (pos, weight)

  HmBrushEditorState() :
    is_drawing(false), is_initted(false), max_height(1), brush_radius(50),
    brush_strength(1), blur_strength(8),
    last_mouse_pos(std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
  { }

  void add_change(hmap::Vec2<float> pos, float weight);

  void apply_pending_changes();
  void apply_brushes(hmap::HeightMap& h, std::span<hmap::Array> kernels, std::span<hmap::Vec2<float>> positions);
};

void hmap_brush_editor(HmBrushEditorState& state, ImTextureID canvas_texture, float width = 0.0f);

} // namespace hesiod::gui