#include <iostream>
#include <memory>
#include <string>

#include "macrologger.h"

#include "gnode.hpp"
// #include "highmap.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

int main()
{
  hmap::Vec2<int> shape = {512, 512};
  hmap::Vec2<int> tiling = {4, 2};
  float           overlap = 0.25;

  // ----------------------------------- Main GUI

  GLFWwindow *window =
      hesiod::gui::init_gui(1400, 800, "Hesiod v0.0.x (c) 2023 Otto Link");
  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  ImNodes::CreateContext();
  ImNodes::StyleColorsDark();
  // ImNodes::StyleColorsClassic();

  hesiod::vnode::ViewTree tree =
      hesiod::vnode::ViewTree("tree_1", shape, tiling, overlap);

  hesiod::vnode::ViewTree tree2 =
      hesiod::vnode::ViewTree("tree_2", shape, tiling, overlap);

  // tree.generate_all_links();
  tree.update();

  tree.print_node_links();

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- GUI content

    tree.render_node_editor();

    // tree2.render_node_editor();

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

  ImNodes::DestroyContext();

  // --- Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  std::cout << "ok" << std::endl;
  return 0;
}
