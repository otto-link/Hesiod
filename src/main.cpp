#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ImCandy/candy.h"
#include "gnode.hpp"
#include "macrologger.h"

#include "hesiod/viewer.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/fonts.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

int main()
{
  hmap::Vec2<int> shape = {512, 512};
  shape = {1024, 1024};

  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.25f;

  // tiling = {1, 1};
  // overlap = 0.f;

  // ----------------------------------- Main GUI

  GLFWwindow *window =
      hesiod::gui::init_gui(1800, 800, "Hesiod v0.0.x (c) 2023 Otto Link");
  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  ImCandy::Theme_Blender();

  ImGuiIO     &io = ImGui::GetIO();
  ImFontConfig config;
  config.OversampleH = 2;
  config.OversampleV = 2;
  io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf,
                                 Roboto_Regular_ttf_len,
                                 16.f,
                                 &config);

  hesiod::vnode::ViewTree tree =
      hesiod::vnode::ViewTree("tree_1", shape, tiling, overlap);

  hesiod::vnode::ViewTree tree2 =
      hesiod::vnode::ViewTree("tree_2", shape, tiling, overlap);

  tree.add_view_node("FbmPerlin");
  // tree.add_view_node("ZeroedEdges");
  // tree.add_view_node("Plateau");
  // tree.add_view_node("Path");
  // tree.add_view_node("PathToHeightmap");
  // tree.new_link("FbmPerlin##0", "output", "Plateau##1", "input");
  // tree.new_link("Path##1", "output", "PathToHeightmap##2", "path");

  // hmap::Cloud cloud = hmap::Cloud(5, 2);
  // hmap::Cloud cloud2 = hmap::Cloud(15, 3);
  // hmap::Path path = hmap::Path(15, 3);
  // path.reorder_nns();

  hmap::HeightMap h = hmap::HeightMap(shape, tiling, overlap);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- GUI content

    hesiod::gui::main_dock(tree);

    tree.render_node_editor();
    // tree2.render_node_editor();

    // ImGui::Begin("TEST");
    // hesiod::gui::hmap_brush_editor(h);
    // ImGui::End();

    // --- Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w,
                 clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // --- Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  std::cout << "ok" << std::endl;
  return 0;
}
