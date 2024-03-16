#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "hesiod/attribute.hpp"
#include "hesiod/view_tree.hpp"
#include "imgui.h"

namespace hesiod::gui
{

enum icon_type : int
{
  circle,
  square
};

void save_screenshot(std::string fname);

// main GUI
void main_dock(hesiod::vnode::ViewTree &view_tree);

// attribute settings
bool render_attribute_widget(std::unique_ptr<Attribute> &p_attr);

/**
 * @brief Render the settings of an attribute (widget type is based on the
 * attribute type).
 * @param p_attr Reference to the attribute.
 * @param label Settings label.
 * @param p_node Reference to the parent node.
 * @return True if the settings have changed.
 */
bool render_attribute_widget(std::unique_ptr<Attribute> &p_attr,
                             std::string                 label);

// custom ImGui widgets
void hmap_cloud(hmap::Cloud &cloud, float width = 0.f, float radius = 4.f);

bool hmap_cloud_editor(hmap::Cloud &cloud,
                       float        width = 0.f,
                       ImVec2      *p_canvas_p0 = nullptr,
                       ImVec2      *p_canvas_size = nullptr);

void hmap_path(hmap::Path &path, float width = 0.f);

bool hmap_path_editor(hmap::Path &path, float width = 0.f);

bool drag_float_matrix(std::vector<std::vector<float>> &matrix);

bool drag_float_vec2(hmap::Vec2<float> &kw, bool &link_xy);

bool drag_float_vector(std::vector<float> &vector,
                       bool                monotonic = false,
                       bool                variable_size = true,
                       float               vmin = 0.f,
                       float               vmax = 1.f,
                       bool                horizontal = false,
                       float               height = 160.f);

void draw_icon(int icon_type, ImVec2 size, ImU32 color, bool filled);

void flip_vertically(int width, int height, uint8_t *data);

void help_marker(std::string text);

bool listbox_map_enum(std::map<std::string, int> &map,
                      int                        &selected,
                      float                       width);

void render_colorbar(std::vector<std::vector<float>> colormap_data,
                     ImVec2                          size);

bool select_shape(std::string      label,
                  hmap::Vec2<int> &shape,
                  hmap::Vec2<int>  shape_max);

bool slider_vmin_vmax(float &vmin, float &vmax);

} // namespace hesiod::gui