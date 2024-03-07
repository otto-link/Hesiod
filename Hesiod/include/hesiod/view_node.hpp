/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <span>
#include <string>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "gnode.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/hmap_brush_editor.hpp"
#include "hesiod/timer.hpp"

namespace hesiod::vnode
{

/**
 * @brief Preview type.
 */
enum preview_type : int
{
  grayscale, ///< grayscale
  jet,       ///< jet
  histogram  ///< histogram
};

struct viewnode_color_set
{
  uint32_t base;
  uint32_t hovered;

  viewnode_color_set(hmap::Vec4<int> rgba)
  {
    this->base = IM_COL32(rgba.a, rgba.b, rgba.c, (int)(0.3f * rgba.d));
    this->hovered = IM_COL32(rgba.a, rgba.b, rgba.c, rgba.d);
  }
};

/* // Nord theme
static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Math", viewnode_color_set({46, 52, 64, 255})},
    {"Geometry", viewnode_color_set({67, 76, 94, 255})},
    {"Roads", viewnode_color_set({76, 86, 106, 255})},
    {"Routing", viewnode_color_set({216, 222, 233, 255})},
    {"IO", viewnode_color_set({229, 233, 240, 255})},
    {"Primitive", viewnode_color_set({143, 188, 187, 255})},
    {"Hydrology", viewnode_color_set({136, 192, 208, 255})},
    {"Operator", viewnode_color_set({129, 161, 193, 255})},
    {"Filter", viewnode_color_set({94, 129, 172, 255})},
    {"Features", viewnode_color_set({191, 97, 106, 255})},
    {"Erosion", viewnode_color_set({235, 203, 139, 255})},
    {"Biomes", viewnode_color_set({163, 190, 140, 255})},
    {"Mask", viewnode_color_set({180, 142, 173, 255})}}; */

/* // OceanicNext theme
static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Math", viewnode_color_set({27, 43, 52, 255})},
    {"Geometry", viewnode_color_set({52, 61, 70, 255})},
    {"Roads", viewnode_color_set({79, 91, 102, 255})},
    {"Filter", viewnode_color_set({101, 115, 126, 255})},
    {"Operator", viewnode_color_set({167, 173, 186, 255})},
    {"Routing", viewnode_color_set({192, 197, 206, 255})},
    {"IO", viewnode_color_set({205, 211, 222, 255})},
    {"Features", viewnode_color_set({249, 145, 87, 255})},
    {"Erosion", viewnode_color_set({250, 200, 99, 255})},
    {"Biomes", viewnode_color_set({153, 199, 148, 255})},
    {"Primitive", viewnode_color_set({98, 179, 178, 255})},
    {"Hydrology", viewnode_color_set({102, 153, 204, 255})},
    {"Mask", viewnode_color_set({197, 148, 197, 255})}}; */

// Solarized
static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Math", viewnode_color_set({0, 43, 54, 255})},
    {"Geometry", viewnode_color_set({101, 123, 131, 255})},
    {"Roads", viewnode_color_set({147, 161, 161, 255})},
    {"Routing", viewnode_color_set({188, 182, 163, 255})},
    {"IO", viewnode_color_set({203, 196, 177, 255})},
    {"Features", viewnode_color_set({181, 137, 0, 255})},
    {"Erosion", viewnode_color_set({203, 75, 22, 255})},
    {"Mask", viewnode_color_set({211, 54, 130, 255})},
    {"Filter", viewnode_color_set({108, 113, 196, 255})},
    {"Operator", viewnode_color_set({108, 113, 196, 255})},
    {"Hydrology", viewnode_color_set({38, 139, 210, 255})},
    {"Primitive", viewnode_color_set({42, 161, 152, 255})},
    {"Biomes", viewnode_color_set({133, 153, 0, 255})},
    {"Texture", viewnode_color_set({0, 0, 0, 255})}};

// Dracula theme for links
static const std::map<int, viewnode_color_set> dtype_colors = {
    {hesiod::cnode::dArray, viewnode_color_set({255, 121, 198, 255})},
    {hesiod::cnode::dCloud, viewnode_color_set({139, 233, 253, 255})},
    {hesiod::cnode::dHeightMap, viewnode_color_set({255, 255, 255, 255})},
    {hesiod::cnode::dHeightMapRGB, viewnode_color_set({255, 184, 108, 255})},
    {hesiod::cnode::dPath, viewnode_color_set({80, 250, 123, 255})}};

/**
 * @brief Base class for all 'View Node' (nodes with a GUI).
 */
class ViewNode : virtual public hesiod::cnode::ControlNode
{
public:
  /**
   * @brief Construct a new View Node object.
   *
   * @param id Node id.
   */
  ViewNode(std::string id);

  /**
   * @brief Get the view3d port id.
   *
   * @return std::string Port id.
   */
  std::string get_preview_port_id();

  /**
   * @brief Get the view3d elevation port id.
   *
   * @return std::string
   */
  std::string get_view3d_elevation_port_id();

  /**
   * @brief Get the view3d color port id.
   *
   * @return std::string
   */
  std::string get_view3d_color_port_id();

  /**
   * @brief Set the preview port id.
   *
   * @param new_port_id New port id.
   */
  void set_preview_port_id(std::string new_port_id);

  /**
   * @brief Set the preview type (grayscale, histogram)
   *
   * @see preview_type
   *
   * @param new_preview_type New preview type.
   */
  void set_preview_type(int new_preview_type);

  /**
   * @brief Set the view3d elevation port id.
   *
   * @param new_port_id
   */
  void set_view3d_elevation_port_id(std::string new_port_id);

  /**
   * @brief Set the view3d color port id.
   *
   * @param new_port_id
   */
  void set_view3d_color_port_id(std::string new_port_id);

  /**
   * @brief Method called before every update of the control node.
   */
  virtual void pre_control_node_update();

  /**
   * @brief Method called after every update of the control node.
   */
  virtual void post_control_node_update();

  /**
   * @brief Render the attribute settings GUI widgets.
   *
   * @return true Settings have changed.
   * @return false Settings have not been changed.
   */
  virtual bool render_attribute_settings();

  /**
   * @brief Render the settings GUI widgets.
   *
   * @return true Settings have changed.
   * @return false Settings have not been changed.
   */
  virtual bool render_settings();

  /**
   * @brief Render the node within the current node editor context.
   */
  void render_node();

  void render_node_minimalist();

  /**
   * @brief Render any specific content after rendering the node base boyd
   * (@link render_node).
   */
  virtual void render_node_specific_content(){};

  /**
   * @brief Render the generic header of the node settings.
   *
   * @return true Settings have changed.
   * @return false Settings have not been changed.
   */
  bool render_settings_header();

  /**
   * @brief Render the generic footer of the node settings.
   *
   * @return true Settings have changed.
   * @return false Settings have not been changed.
   */
  bool render_settings_footer();

  /**
   * @brief To be placed after an ImGui widget, triggers the node update after
   * the widget has been edited (and true when this is the case).
   *
   * @return true Widget has been edited.
   * @return false Has not.
   */
  bool trigger_update_after_edit();

  /**
   * @brief Update the node preview (regenerate the content displayed in the
   * node body).
   */
  void update_preview();

protected:
  /**
   * @brief Port id of the data displayed in the preview.
   */
  std::string preview_port_id = "";

  /**
   * @brief Port id of the elevation data displayed in the 3D viewer.
   */
  std::string view3d_elevation_port_id = "";

  /**
   * @brief Port id of the color data displayed in the 3D viewer.
   */
  std::string view3d_color_port_id = "";

  /**
   * @brief Defines whether the preview is shown or not in the node body.
   */
  bool show_preview = true;

  /**
   * @brief Node width in pixels.
   */
  float node_width = 128.f;

  /**
   * @brief Defines whether the user help is shown or not in the settings body.
   */
  bool show_help = false;

  /**
   * @brief Default help text.
   */
  std::string help_text = "No help available.";

  /**
   * @brief OpenGL texture used to store the image preview.
   */
  GLuint image_texture_preview = 0;

private:
  /**
   * @brief Shape preview.
   */
  hmap::Vec2<int> shape_preview = {128, 128};

  /**
   * @brief Preview type.
   */
  int preview_type = preview_type::grayscale;

  /**
   * @brief Set the control node post-update callback to the view node
   * post-update method.
   */
  void init_from_control_node();

  /**
   * @brief Timer instance use to measure update time of the node.
   */
  hesiod::Timer timer = hesiod::Timer();

  /**
   * @brief Update time of the node (in milliseconds).
   */
  float update_time = 0.f;
};

//----------------------------------------
// End-user nodes
//----------------------------------------

class ViewAbs : public ViewNode, public hesiod::cnode::Abs
{
public:
  ViewAbs(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Abs(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewAbsSmooth : public ViewNode, public hesiod::cnode::AbsSmooth
{
public:
  ViewAbsSmooth(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::AbsSmooth(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewBaseElevation : public ViewNode, public hesiod::cnode::BaseElevation
{
public:
  ViewBaseElevation(std::string     id,
                    hmap::Vec2<int> shape,
                    hmap::Vec2<int> tiling,
                    float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::BaseElevation(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewBezierPath : public ViewNode, public hesiod::cnode::BezierPath
{
public:
  ViewBezierPath(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::BezierPath(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewBiquadPulse : public ViewNode, public hesiod::cnode::BiquadPulse
{
public:
  ViewBiquadPulse(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::BiquadPulse(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewBlend : public ViewNode, public hesiod::cnode::Blend
{
public:
  ViewBlend(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Blend(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewBrush : public ViewNode, public hesiod::cnode::Brush
{
public:
  ViewBrush(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  bool render_settings();

  void sync_drawing_texture();
  void sync_value();

private:
  hesiod::gui::HmBrushEditorState edit_state;
};

class ViewBump : public ViewNode, public hesiod::cnode::Bump
{
public:
  ViewBump(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Bump(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewCaldera : public ViewNode, public hesiod::cnode::Caldera
{
public:
  ViewCaldera(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Caldera(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewCheckerboard : public ViewNode, public hesiod::cnode::Checkerboard
{
public:
  ViewCheckerboard(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Checkerboard(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewClamp : public ViewNode, public hesiod::cnode::Clamp
{
public:
  ViewClamp(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Clamp(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewClone : public ViewNode, public hesiod::cnode::Clone
{
public:
  ViewClone(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Clone(id)
  {
  }

  bool render_settings();
};

class ViewCloud : public ViewNode, public hesiod::cnode::Cloud
{
public:
  ViewCloud(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Cloud(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewCloudToArrayInterp : public ViewNode,
                               public hesiod::cnode::CloudToArrayInterp
{
public:
  ViewCloudToArrayInterp(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::CloudToArrayInterp(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewColorize : public ViewNode, public hesiod::cnode::Colorize
{
public:
  ViewColorize(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Colorize(id)
  {
    this->set_preview_port_id("RGB");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("RGB");
  }

  bool render_settings();
};

class ViewColorizeSolid : public ViewNode, public hesiod::cnode::ColorizeSolid
{
public:
  ViewColorizeSolid(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ColorizeSolid(id)
  {
    this->set_preview_port_id("RGB");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("RGB");
  }
};

class ViewCombineMask : public ViewNode, public hesiod::cnode::CombineMask
{
public:
  ViewCombineMask(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::CombineMask(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_color_port_id("output");
  }
};

class ViewConvolveSVD : public ViewNode, public hesiod::cnode::ConvolveSVD
{
public:
  ViewConvolveSVD(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ConvolveSVD(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewDebug : public ViewNode, public hesiod::cnode::Debug
{
public:
  std::string log = "";
  bool        auto_export_png = false;

  ViewDebug(std::string id);

  void export_to_png();

  void post_control_node_update();

  bool render_settings();

  void render_node_specific_content();
};

class ViewDendry : public ViewNode, public hesiod::cnode::Dendry
{
public:
  ViewDendry(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Dendry(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewDepressionFilling : public ViewNode,
                              public hesiod::cnode::DepressionFilling
{
public:
  ViewDepressionFilling(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::DepressionFilling(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewDigPath : public ViewNode, public hesiod::cnode::DigPath
{
public:
  ViewDigPath(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::DigPath(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewDistanceTransform : public ViewNode,
                              public hesiod::cnode::DistanceTransform
{
public:
  ViewDistanceTransform(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::DistanceTransform(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
    this->help_text = "Apply the distance transform (Euclidean distance) to "
                      "the input heightmap. Cells with null or negative values "
                      "are assumed to belong to the background.";
  }
};

class ViewEqualize : public ViewNode, public hesiod::cnode::Equalize
{
public:
  ViewEqualize(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Equalize(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewErosionMaps : public ViewNode, public hesiod::cnode::ErosionMaps
{
public:
  ViewErosionMaps(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ErosionMaps(id)
  {
  }
};

class ViewExpandShrink : public ViewNode, public hesiod::cnode::ExpandShrink
{
public:
  ViewExpandShrink(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ExpandShrink(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewExpandShrinkDirectional
    : public ViewNode,
      public hesiod::cnode::ExpandShrinkDirectional
{
public:
  ViewExpandShrinkDirectional(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ExpandShrinkDirectional(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewExport : public ViewNode, public hesiod::cnode::Export
{
public:
  ViewExport(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Export(id)
  {
    this->set_preview_port_id("input");
    this->set_view3d_elevation_port_id("input");
  }

  void render_node_specific_content();
};

class ViewExportRGB : public ViewNode, public hesiod::cnode::ExportRGB
{
public:
  ViewExportRGB(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ExportRGB(id)
  {
    this->set_preview_port_id("RGB");
  }

  void render_node_specific_content();
};

class ViewFaceted : public ViewNode, public hesiod::cnode::Faceted
{
public:
  ViewFaceted(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Faceted(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmIqPerlin : public ViewNode, public hesiod::cnode::FbmIqPerlin
{
public:
  ViewFbmIqPerlin(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmIqPerlin(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmPerlin : public ViewNode, public hesiod::cnode::FbmPerlin
{
public:
  ViewFbmPerlin(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmPerlin(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmPingpongPerlin : public ViewNode,
                              public hesiod::cnode::FbmPingpongPerlin
{
public:
  ViewFbmPingpongPerlin(std::string     id,
                        hmap::Vec2<int> shape,
                        hmap::Vec2<int> tiling,
                        float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmPingpongPerlin(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmRidgedPerlin : public ViewNode,
                            public hesiod::cnode::FbmRidgedPerlin
{
public:
  ViewFbmRidgedPerlin(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmRidgedPerlin(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmSimplex : public ViewNode, public hesiod::cnode::FbmSimplex
{
public:
  ViewFbmSimplex(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmSimplex(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmWorley : public ViewNode, public hesiod::cnode::FbmWorley
{
public:
  ViewFbmWorley(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmWorley(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFbmWorleyDouble : public ViewNode,
                            public hesiod::cnode::FbmWorleyDouble
{
public:
  ViewFbmWorleyDouble(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FbmWorleyDouble(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewFractalizePath : public ViewNode, public hesiod::cnode::FractalizePath
{
public:
  ViewFractalizePath(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::FractalizePath(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewGaborNoise : public ViewNode, public hesiod::cnode::GaborNoise
{
public:
  ViewGaborNoise(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GaborNoise(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewGain : public ViewNode, public hesiod::cnode::Gain
{
public:
  ViewGain(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Gain(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewGammaCorrection : public ViewNode,
                            public hesiod::cnode::GammaCorrection
{
public:
  ViewGammaCorrection(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GammaCorrection(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewGammaCorrectionLocal : public ViewNode,
                                 public hesiod::cnode::GammaCorrectionLocal
{
public:
  ViewGammaCorrectionLocal(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GammaCorrectionLocal(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewGeomorphons : public ViewNode, public hesiod::cnode::Geomorphons
{
public:
  ViewGeomorphons(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Geomorphons(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewGradient : public ViewNode, public hesiod::cnode::Gradient
{
public:
  ViewGradient(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Gradient(id)
  {
  }
};

class ViewGradientAngle : public ViewNode, public hesiod::cnode::GradientAngle
{
public:
  ViewGradientAngle(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GradientAngle(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewGradientNorm : public ViewNode, public hesiod::cnode::GradientNorm
{
public:
  ViewGradientNorm(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GradientNorm(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewGradientTalus : public ViewNode, public hesiod::cnode::GradientTalus
{
public:
  ViewGradientTalus(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GradientTalus(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewGaussianPulse : public ViewNode, public hesiod::cnode::GaussianPulse
{
public:
  ViewGaussianPulse(std::string     id,
                    hmap::Vec2<int> shape,
                    hmap::Vec2<int> tiling,
                    float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::GaussianPulse(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewHydraulicAlgebric : public ViewNode,
                              public hesiod::cnode::HydraulicAlgebric
{
public:
  ViewHydraulicAlgebric(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::HydraulicAlgebric(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewHydraulicParticle : public ViewNode,
                              public hesiod::cnode::HydraulicParticle
{
public:
  ViewHydraulicParticle(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::HydraulicParticle(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewHydraulicRidge : public ViewNode, public hesiod::cnode::HydraulicRidge
{
public:
  ViewHydraulicRidge(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::HydraulicRidge(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewHydraulicStream : public ViewNode,
                            public hesiod::cnode::HydraulicStream
{
public:
  ViewHydraulicStream(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::HydraulicStream(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewHydraulicStreamLog : public ViewNode,
                               public hesiod::cnode::HydraulicStreamLog
{
public:
  ViewHydraulicStreamLog(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::HydraulicStreamLog(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewHydraulicVpipes : public ViewNode,
                            public hesiod::cnode::HydraulicVpipes
{
public:
  ViewHydraulicVpipes(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::HydraulicVpipes(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewImport : public ViewNode, public hesiod::cnode::Import
{
public:
  ViewImport(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Import(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }

  void render_node_specific_content();
};

class ViewInverse : public ViewNode, public hesiod::cnode::Inverse
{
public:
  ViewInverse(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Inverse(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewKernel : public ViewNode, public hesiod::cnode::Kernel
{
public:
  ViewKernel(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Kernel(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewKmeansClustering2 : public ViewNode,
                              public hesiod::cnode::KmeansClustering2
{
public:
  ViewKmeansClustering2(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::KmeansClustering2(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewKmeansClustering3 : public ViewNode,
                              public hesiod::cnode::KmeansClustering3
{
public:
  ViewKmeansClustering3(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::KmeansClustering3(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewLaplace : public ViewNode, public hesiod::cnode::Laplace
{
public:
  ViewLaplace(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Laplace(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewLaplaceEdgePreserving : public ViewNode,
                                  public hesiod::cnode::LaplaceEdgePreserving
{
public:
  ViewLaplaceEdgePreserving(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::LaplaceEdgePreserving(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewLerp : public ViewNode, public hesiod::cnode::Lerp
{
public:
  ViewLerp(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Lerp(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewMakeBinary : public ViewNode, public hesiod::cnode::MakeBinary

{
public:
  ViewMakeBinary(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::MakeBinary(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewMeanderizePath : public ViewNode, public hesiod::cnode::MeanderizePath
{
public:
  ViewMeanderizePath(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::MeanderizePath(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewMeanLocal : public ViewNode, public hesiod::cnode::MeanLocal
{
public:
  ViewMeanLocal(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::MeanLocal(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewMedian3x3 : public ViewNode, public hesiod::cnode::Median3x3
{
public:
  ViewMedian3x3(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Median3x3(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewMinimumLocal : public ViewNode, public hesiod::cnode::MinimumLocal
{
public:
  ViewMinimumLocal(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::MinimumLocal(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewMixRGB : public ViewNode, public hesiod::cnode::MixRGB
{
public:
  ViewMixRGB(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::MixRGB(id)
  {
    this->set_preview_port_id("RGB");
  }
};

class ViewNoise : public ViewNode, public hesiod::cnode::Noise
{
public:
  ViewNoise(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Noise(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
    this->help_text = "Generate a coherent noise (Perlin, Worley...).";
  }
};

class ViewNormalDisplacement : public ViewNode,
                               public hesiod::cnode::NormalDisplacement
{
public:
  ViewNormalDisplacement(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::NormalDisplacement(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewOneMinus : public ViewNode, public hesiod::cnode::OneMinus
{
public:
  ViewOneMinus(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::OneMinus(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewPath : public ViewNode, public hesiod::cnode::Path
{
public:
  ViewPath(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Path(id)
  {
    this->set_preview_port_id("output");
  }
};

class ViewPathFinding : public ViewNode, public hesiod::cnode::PathFinding
{
public:
  ViewPathFinding(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::PathFinding(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("heightmap");
    this->set_view3d_color_port_id("output");
    this->help_text =
        "Find the lowest elevation and elevation difference path for each edge "
        "of the input path, assuming this path lies on the input "
        "heightmap.\n'elevation_ratio': Balance between absolute elevation and "
        "elevation difference in the cost function.\n'distance_exponent': "
        "Exponent of the distance calculation between two points. Increasing "
        "the distance exponent of the cost function increases the cost of "
        "elevation gaps: path then tends to stay at the same elevation if "
        "possible (i.e. reduce the overall cumulative elevation gain).";
  }
};

class ViewPathToHeightmap : public ViewNode,
                            public hesiod::cnode::PathToHeightmap
{
public:
  ViewPathToHeightmap(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::PathToHeightmap(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("path");
    this->set_view3d_elevation_port_id("output");
    this->set_view3d_color_port_id("path");
  }
};

class ViewPeak : public ViewNode, public hesiod::cnode::Peak
{
public:
  ViewPeak(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Peak(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewPerlin : public ViewNode, public hesiod::cnode::Perlin
{
public:
  ViewPerlin(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Perlin(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
    this->help_text = "Generate a Perlin noise.";
  }
};

class ViewPerlinBillow : public ViewNode, public hesiod::cnode::PerlinBillow
{
public:
  ViewPerlinBillow(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::PerlinBillow(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewPlateau : public ViewNode, public hesiod::cnode::Plateau
{
public:
  ViewPlateau(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Plateau(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewPreview : public ViewNode, public hesiod::cnode::Preview
{
public:
  ViewPreview(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Preview(id)
  {
    this->set_preview_port_id("input");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("color mask");
  }
};

class ViewPreviewColorize : public ViewNode,
                            public hesiod::cnode::PreviewColorize
{
public:
  ViewPreviewColorize(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::PreviewColorize(id)
  {
    this->set_preview_port_id("RGB");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("RGB");
  }
};

class ViewRecastCanyon : public ViewNode, public hesiod::cnode::RecastCanyon
{
public:
  ViewRecastCanyon(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecastCanyon(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecastCliff : public ViewNode, public hesiod::cnode::RecastCliff
{
public:
  ViewRecastCliff(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecastCliff(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecastCliffDirectional : public ViewNode,
                                   public hesiod::cnode::RecastCliffDirectional
{
public:
  ViewRecastCliffDirectional(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecastCliffDirectional(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecastPeak : public ViewNode, public hesiod::cnode::RecastPeak
{
public:
  ViewRecastPeak(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecastPeak(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecastRockySlopes : public ViewNode,
                              public hesiod::cnode::RecastRockySlopes
{
public:
  ViewRecastRockySlopes(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecastRockySlopes(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecurve : public ViewNode, public hesiod::cnode::Recurve
{
public:
  ViewRecurve(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Recurve(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecurveKura : public ViewNode, public hesiod::cnode::RecurveKura
{
public:
  ViewRecurveKura(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecurveKura(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRecurveS : public ViewNode, public hesiod::cnode::RecurveS
{
public:
  ViewRecurveS(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RecurveS(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRelativeElevation : public ViewNode,
                              public hesiod::cnode::RelativeElevation
{
public:
  ViewRelativeElevation(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::RelativeElevation(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRemap : public ViewNode, public hesiod::cnode::Remap
{
public:
  ViewRemap(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Remap(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRescale : public ViewNode, public hesiod::cnode::Rescale
{
public:
  ViewRescale(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Rescale(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewRugosity : public ViewNode, public hesiod::cnode::Rugosity
{
public:
  ViewRugosity(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Rugosity(id)
  {
    LOG_DEBUG("ViewRugosity::ViewRugosity");
    LOG_DEBUG("node [%s]", this->id.c_str());
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSedimentDeposition : public ViewNode,
                               public hesiod::cnode::SedimentDeposition
{
public:
  ViewSedimentDeposition(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SedimentDeposition(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewSelectBlobLog : public ViewNode, public hesiod::cnode::SelectBlobLog
{
public:
  ViewSelectBlobLog(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectBlobLog(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectCavities : public ViewNode, public hesiod::cnode::SelectCavities
{
public:
  ViewSelectCavities(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectCavities(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectElevationSlope : public ViewNode,
                                 public hesiod::cnode::SelectElevationSlope
{
public:
  ViewSelectElevationSlope(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectElevationSlope(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectEq : public ViewNode, public hesiod::cnode::SelectEq
{
public:
  ViewSelectEq(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectEq(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }

  bool render_settings();

private:
  bool               use_input_unique_values = false;
  std::vector<float> input_unique_values = {};
  int                selected_idx = 0;
};

class ViewSelectGradientNorm : public ViewNode,
                               public hesiod::cnode::SelectGradientNorm
{
public:
  ViewSelectGradientNorm(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectGradientNorm(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectInterval : public ViewNode, public hesiod::cnode::SelectInterval
{
public:
  ViewSelectInterval(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectInterval(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectPulse : public ViewNode, public hesiod::cnode::SelectPulse
{
public:
  ViewSelectPulse(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectPulse(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectRivers : public ViewNode, public hesiod::cnode::SelectRivers
{
public:
  ViewSelectRivers(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectRivers(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSelectTransitions : public ViewNode,
                              public hesiod::cnode::SelectTransitions
{
public:
  ViewSelectTransitions(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SelectTransitions(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("blend");
    this->set_view3d_color_port_id("output");
  }
};

class ViewSimplex : public ViewNode, public hesiod::cnode::Simplex
{
public:
  ViewSimplex(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Simplex(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
    this->help_text = "Generate a Simplex noise.";
  }
};

class ViewSlope : public ViewNode, public hesiod::cnode::Slope
{
public:
  ViewSlope(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Slope(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewSmoothCpulse : public ViewNode, public hesiod::cnode::SmoothCpulse
{
public:
  ViewSmoothCpulse(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SmoothCpulse(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewSmoothFill : public ViewNode, public hesiod::cnode::SmoothFill
{
public:
  ViewSmoothFill(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SmoothFill(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewSmoothFillHoles : public ViewNode,
                            public hesiod::cnode::SmoothFillHoles
{
public:
  ViewSmoothFillHoles(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SmoothFillHoles(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewSmoothFillSmearPeaks : public ViewNode,
                                 public hesiod::cnode::SmoothFillSmearPeaks
{
public:
  ViewSmoothFillSmearPeaks(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SmoothFillSmearPeaks(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewSteepenConvective : public ViewNode,
                              public hesiod::cnode::SteepenConvective
{
public:
  ViewSteepenConvective(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::SteepenConvective(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewStep : public ViewNode, public hesiod::cnode::Step
{
public:
  ViewStep(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Step(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewStratifyMultiscale : public ViewNode,
                               public hesiod::cnode::StratifyMultiscale
{
public:
  ViewStratifyMultiscale(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::StratifyMultiscale(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }

  bool render_settings();
};

class ViewStratifyOblique : public ViewNode,
                            public hesiod::cnode::StratifyOblique
{
public:
  ViewStratifyOblique(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::StratifyOblique(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewThermal : public ViewNode, public hesiod::cnode::Thermal
{
public:
  ViewThermal(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Thermal(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewThermalAutoBedrock : public ViewNode,
                               public hesiod::cnode::ThermalAutoBedrock
{
public:
  ViewThermalAutoBedrock(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ThermalAutoBedrock(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewThermalFlatten : public ViewNode, public hesiod::cnode::ThermalFlatten
{
public:
  ViewThermalFlatten(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ThermalFlatten(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewThermalScree : public ViewNode, public hesiod::cnode::ThermalScree
{
public:
  ViewThermalScree(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ThermalScree(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewToKernel : public ViewNode, public hesiod::cnode::ToKernel
{
public:
  ViewToKernel(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ToKernel(id)
  {
    this->set_preview_port_id("kernel");
  }
};

class ViewToMask : public ViewNode, public hesiod::cnode::ToMask
{
public:
  ViewToMask(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::ToMask(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewValleyWidth : public ViewNode, public hesiod::cnode::ValleyWidth
{
public:
  ViewValleyWidth(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ValleyWidth(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("input");
    this->set_view3d_color_port_id("output");
  }
};

class ViewValueNoiseDelaunay : public ViewNode,
                               public hesiod::cnode::ValueNoiseDelaunay
{
public:
  ViewValueNoiseDelaunay(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ValueNoiseDelaunay(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewValueNoiseLinear : public ViewNode,
                             public hesiod::cnode::ValueNoiseLinear
{
public:
  ViewValueNoiseLinear(std::string     id,
                       hmap::Vec2<int> shape,
                       hmap::Vec2<int> tiling,
                       float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ValueNoiseLinear(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewValueNoiseThinplate : public ViewNode,
                                public hesiod::cnode::ValueNoiseThinplate
{
public:
  ViewValueNoiseThinplate(std::string     id,
                          hmap::Vec2<int> shape,
                          hmap::Vec2<int> tiling,
                          float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ValueNoiseThinplate(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWarp : public ViewNode, public hesiod::cnode::Warp
{
public:
  ViewWarp(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Warp(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWarpDownslope : public ViewNode, public hesiod::cnode::WarpDownslope
{
public:
  ViewWarpDownslope(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WarpDownslope(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWaveDune : public ViewNode, public hesiod::cnode::WaveDune
{
public:
  ViewWaveDune(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WaveDune(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWaveSine : public ViewNode, public hesiod::cnode::WaveSine
{
public:
  ViewWaveSine(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WaveSine(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWaveSquare : public ViewNode, public hesiod::cnode::WaveSquare
{
public:
  ViewWaveSquare(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WaveSquare(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWaveTriangular : public ViewNode, public hesiod::cnode::WaveTriangular
{
public:
  ViewWaveTriangular(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WaveTriangular(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWhite : public ViewNode, public hesiod::cnode::White
{
public:
  ViewWhite(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::White(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWhiteDensityMap : public ViewNode,
                            public hesiod::cnode::WhiteDensityMap
{
public:
  ViewWhiteDensityMap(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WhiteDensityMap(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWhiteSparse : public ViewNode, public hesiod::cnode::WhiteSparse
{
public:
  ViewWhiteSparse(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WhiteSparse(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWorley : public ViewNode, public hesiod::cnode::Worley
{
public:
  ViewWorley(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::Worley(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWorleyDouble : public ViewNode, public hesiod::cnode::WorleyDouble
{
public:
  ViewWorleyDouble(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WorleyDouble(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWorleyValue : public ViewNode, public hesiod::cnode::WorleyValue
{
public:
  ViewWorleyValue(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::WorleyValue(id, shape, tiling, overlap)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewWrinkle : public ViewNode, public hesiod::cnode::Wrinkle
{
public:
  ViewWrinkle(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Wrinkle(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

class ViewZeroedEdges : public ViewNode, public hesiod::cnode::ZeroedEdges
{
public:
  ViewZeroedEdges(std::string id)
      : hesiod::cnode::ControlNode(id), ViewNode(id),
        hesiod::cnode::ZeroedEdges(id)
  {
    this->set_preview_port_id("output");
    this->set_view3d_elevation_port_id("output");
  }
};

// // HELPERS

void img_to_texture(std::vector<uint8_t> img,
                    hmap::Vec2<int>      shape,
                    GLuint              &image_texture);

void img_to_texture_rgb(std::vector<uint8_t> img,
                        hmap::Vec2<int>      shape,
                        GLuint              &image_texture);

} // namespace hesiod::vnode
