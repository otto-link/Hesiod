#include "highmap/geometry.hpp"
#define _USE_MATH_DEFINES
typedef unsigned int uint;

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

#include "hesiod/attribute.hpp"
#include "hesiod/serialization.hpp"

#if ENABLE_GENERATE_NODE_SNAPSHOT
#include "generate_node_snapshot.hpp"
#endif

int main(int argc, char *argv[])
{

#if ENABLE_GENERATE_NODE_SNAPSHOT
  if (argc >= 2 && strcmp(argv[1], "--generate-node-snapshot") == 0)
  {
    generate_node_snapshot();
    return 0;
  }
#endif

  if (argc >= 2 && strcmp(argv[1], "--test") == 0)
  {
    nlohmann::json data = nlohmann::json();

    return 0;
  }

  hmap::Vec2<int> shape = {1024, 1024};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.25f;

  // ----------------------------------- Main GUI

  GLFWwindow *window =
      hesiod::gui::init_gui(1800, 800, "Hesiod v0.0.x (c) 2023 Otto Link");
  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  ImCandy::Theme_Blender();

  ImGuiIO     &io = ImGui::GetIO();
  ImFontConfig config;
  config.OversampleH = 2;
  config.OversampleV = 2;
  config.FontDataOwnedByAtlas = false;
  io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf,
                                 Roboto_Regular_ttf_len,
                                 16.f,
                                 &config);

  hesiod::vnode::ViewTree tree =
      hesiod::vnode::ViewTree("tree_1", shape, tiling, overlap);

  tree.add_view_node("FbmSimplex");
  tree.add_view_node("Perlin");

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- GUI content

    hesiod::gui::main_dock(tree);
    tree.render_node_editor();

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
