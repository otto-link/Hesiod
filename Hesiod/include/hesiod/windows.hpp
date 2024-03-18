#pragma once
#include <list>
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "hesiod/shortcuts.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::gui
{
using WindowTag = unsigned int;

// forward declarations
class WindowManager;

class Window : public ShortcutGroupElement
{
public:
  Window();

  std::string get_unique_id();

  WindowManager *get_window_manager_ref()
  {
    return this->p_parent_window_manager;
  }

  void set_tag(WindowTag new_tag)
  {
    this->tag = new_tag;
  }

  std::string get_title() const
  {
    return this->title;
  }

  virtual bool initialize() = 0;

  bool render();

  virtual bool render_content() = 0;

  bool add_shortcuts();

  bool remove_shortcuts();

protected:
  friend class WindowManager;

  WindowTag                              tag = -1;
  std::string                            title;
  ImGuiWindowFlags                       flags;
  WindowManager                         *p_parent_window_manager;
  std::vector<std::unique_ptr<Shortcut>> shortcuts;
  bool                                   is_open = true;
};

class WindowManager
{
public:
  static constexpr WindowTag kInvalidTag = (WindowTag)-1;

  WindowManager();

  WindowTag get_new_tag();
  bool      has_window(WindowTag tag);
  Window   *get_window_ref_by_tag(WindowTag tag);
  bool      remove_window(WindowTag tag);
  bool      remove_all_windows();

  WindowTag add_window_with_tag(WindowTag               tag,
                                std::unique_ptr<Window> p_window);
  WindowTag add_window(std::unique_ptr<Window> p_window);
  bool      do_delete_queue();
  bool      render_windows();

  void handle_input(int key, int scancode, int action, int modifiers);

  ShortcutsManager *get_shortcuts_manager_ref();

private:
  friend class MainWindow;

  std::map<WindowTag, std::unique_ptr<Window>> windows;
  std::list<WindowTag>                         windows_delete_queue;
  WindowTag                                    tag_count = 0;

  ShortcutsManager shortcuts_manager;
};

class MainWindow
{
public:
  MainWindow(std::string window_title, int window_width, int window_height);

  ~MainWindow();

  bool initialize();

  bool shutdown();

  void render_main_menu();

  bool run();

  bool set_title(std::string title);

  bool set_size(int width, int height);

  bool is_running()
  {
    return this->running;
  }

  std::string get_title();

  bool get_size(int &width, int &height);

  WindowManager *get_window_manager_ref();

protected:
  void cleanup_internally();

private:
  std::string title;
  int         width;
  int         height;

  GLFWwindow *p_glfw_window;
  bool        running;

  ImColor clear_color;

  WindowManager window_manager;
};

//----------------------------------------
// Window specializations
//----------------------------------------

/**
 * @brief NodeEditor class, GUI window to manipulate graph node structure.
 */
class NodeEditor : public Window
{
public:
  NodeEditor(hesiod::vnode::ViewTree *p_vtree) : p_vtree(p_vtree)
  {
  }

  ShortcutGroupId get_element_shortcut_group_id() override;

  bool initialize() override;

  bool render_content() override;

  // --- specific to the editor

  void automatic_node_layout();

  ax::NodeEditor::EditorContext *get_p_node_editor_context() const;

  void insert_clone_node(std::string node_id);

  void render_links();

  std::string render_new_node_popup();

  std::string render_new_node_treeview(const ImVec2 node_position = {0.f, 0.f});

  void render_view_nodes();

  void render_view_node(std::string node_id);

private:
  hesiod::vnode::ViewTree *p_vtree;

  // --- related to imgui-node-editor

  ax::NodeEditor::NodeId context_menu_node_hid;

  std::vector<std::string> key_sort;

  std::vector<ax::NodeEditor::NodeId> selected_node_hid = {};

  ax::NodeEditor::EditorContext *p_node_editor_context = nullptr;
};

/**
 * @brief NodeList class, GUI window to display textual informations about the
 * graph node structure.
 */
class NodeList : public Window
{
public:
  NodeList(hesiod::vnode::ViewTree *p_vtree) : p_vtree(p_vtree)
  {
  }

  ShortcutGroupId get_element_shortcut_group_id() override;

  bool initialize() override;

  bool render_content() override;

private:
  hesiod::vnode::ViewTree *p_vtree;
};

/**
 * @brief NodeSettings class, GUI window to display settings widgets of the
 * currently selected nodes.
 */
class NodeSettings : public Window
{
public:
  NodeSettings(hesiod::vnode::ViewTree *p_vtree) : p_vtree(p_vtree)
  {
  }

  ShortcutGroupId get_element_shortcut_group_id() override;

  bool initialize() override;

  bool render_content() override;

private:
  hesiod::vnode::ViewTree *p_vtree;
};

/**
 * @brief Viewer2D class, GUI window to display heightmap as 2D heatmap plots.
 */
class Viewer2D : public Window
{
public:
  /**
   * @brief Constructor.
   * @param p_vtree Reference to the related ViewTree.
   */
  Viewer2D(hesiod::vnode::ViewTree *p_vtree);

  /**
   * @brief Destructor.
   */
  ~Viewer2D();

  ShortcutGroupId get_element_shortcut_group_id() override;

  /**
   * @brief Initialize the window.
   * @return Success.
   */
  bool initialize() override;

  /**
   * @brief Render the window content.
   * @return Success.
   */
  bool render_content() override;

  // --- specific to the 2D viewer

  /**
   * @brief Method called after the node tree has been updated.
   */
  void post_update();

  /**
   * @brief Update the rendered image.
   */
  void update_image_texture();

private:
  /**
   * @brief Reference to the related ViewTree.
   */
  hesiod::vnode::ViewTree *p_vtree;

  // --- viewer parameters

  /**
   * @brief
   */
  std::string viewer_node_id = "";

  /**
   * @brief Whether or not the viewer adjust the content rendered to the
   * currently selected node (or always keep the same node displayed).
   */
  bool pin_current_node = false;

  /**
   * @brief OpenGL texture.
   */
  GLuint image_texture;

  /**
   * @brief Texture default shape.
   */
  hmap::Vec2<int> display_shape = {512, 512};

  /**
   * @brief Colormap.
   */
  int cmap = hmap::cmap::inferno;

  /**
   * @brief Whether or not hillshading is applied to the heatmap.
   */
  bool hillshade = false;

  /**
   * @brief Zoom in %.
   */
  float zoom = 100.f;

  /**
   * @brief Texture translation (use to move the heatmap).
   */
  ImVec2 uv0 = {0.f, 0.f};

  /**
   * @brief Correspondance between the colormap (as an enum) and the colormap
   * (as plain text).
   */
  std::map<std::string, int> cmap_map = {
      {"gray", hmap::cmap::gray},
      {"inferno", hmap::cmap::inferno},
      {"nipy_spectral", hmap::cmap::nipy_spectral},
      {"terrain", hmap::cmap::terrain}};
};

/**
 * @brief Viewer3D class, GUI window for 3D rendering.
 */
class Viewer3D : public Window
{
public:
  /**
   * @brief Constructor.
   * @param p_vtree Reference to the related ViewTree.
   */
  Viewer3D(hesiod::vnode::ViewTree *p_vtree);

  /**
   * @brief Destructor.
   */
  ~Viewer3D();

  ShortcutGroupId get_element_shortcut_group_id() override;

  /**
   * @brief Initialize the window.
   * @return Success.
   */
  bool initialize() override;

  /**
   * @brief Render the window content.
   * @return Success.
   */
  bool render_content() override;

  // --- specific to the 3D viewer

  /**
   * @brief Method called after the node tree has been updated.
   */
  void post_update();

  /**
   * @brief Update (regenerate) a triangle mesh of the input heighmap.
   */
  void update_basemesh();

  /**
   * @brief Update the rendered image.
   * @param vertex_array_update Whether the triangle mesh needs to be updated
   * before rendering the view.
   */
  void update_image_texture(bool vertex_array_update = true);

private:
  /**
   * @brief Reference to the related ViewTree.
   */
  hesiod::vnode::ViewTree *p_vtree;

  // --- viewer parameters

  /**
   * @brief
   */
  std::string viewer_node_id = "";

  /**
   * @brief Whether or not the viewer adjust the content rendered to the
   * currently selected node (or always keep the same node displayed).
   */
  bool pin_current_node = false;

  /**
   * @brief OpenGL texture.
   */
  GLuint image_texture;

  /**
   * @brief Texture default shape.
   */
  hmap::Vec2<int> display_shape = {512, 512};

  // --- rendering stuffs...

  GLuint               shader_id;
  GLuint               vertex_array_id;
  GLuint               vertex_buffer;
  GLuint               color_buffer;
  GLuint               FBO;
  GLuint               RBO;
  std::vector<GLfloat> vertices = {};
  std::vector<GLfloat> colors = {};
  ImColor              clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.f);

  float scale = 0.7f;
  float h_scale = 0.4f;
  float alpha_x = 35.f;
  float alpha_y = -25.f;
  float delta_x = 0.f;
  float delta_y = 0.f;
  bool  wireframe = false;
  bool  auto_rotate = false;

  float fov = 60.0f;
  float aspect_ratio = 1.f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;
};

} // namespace hesiod::gui