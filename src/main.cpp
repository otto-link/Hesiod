#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "macrologger.h"

#include "gnode.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

int main()
{
  hmap::Vec2<int> shape = {512, 512};

  hmap::Vec2<int> tiling = {4, 2};
  float           overlap = 0.25f;

  // hmap::Vec2<int> tiling = {1, 1};
  // float           overlap = 0.f;

  // ----------------------------------- Main GUI

  GLFWwindow *window =
      hesiod::gui::init_gui(1800, 800, "Hesiod v0.0.x (c) 2023 Otto Link");
  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontDefault();

  hesiod::vnode::ViewTree tree =
      hesiod::vnode::ViewTree("tree_1", shape, tiling, overlap);

  hesiod::vnode::ViewTree tree2 =
      hesiod::vnode::ViewTree("tree_2", shape, tiling, overlap);

  tree.add_view_node("Perlin");
  tree.add_view_node("FbmPerlin");
  tree.add_view_node("Gain");

  // tree.generate_all_links();
  tree.update();

  tree.print_node_links();

  hmap::Cloud cloud = hmap::Cloud(5, 2);
  hmap::Cloud cloud2 = hmap::Cloud(15, 3);

  for (auto &p : cloud.points)
    std::cout << p.x << " " << p.y << "\n";

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- GUI content

    tree.render_node_editor();
    // tree2.render_node_editor();

    // ImGui::Begin("test Point Editor");
    // if (hesiod::gui::canvas_cloud_editor(cloud))
    //   cloud.print();
    // ImGui::End();

    // ImGui::Begin("test Point Editor2");
    // if (hesiod::gui::canvas_cloud_editor(cloud2))
    //   cloud2.print();
    // ImGui::End();

    // ImGui::Begin("FPS");
    // auto &io = ImGui::GetIO();
    // ImGui::Text("FPS: %.2f (%.2gms)",
    //             io.Framerate,
    //             io.Framerate ? 1000.0f / io.Framerate : 0.0f);
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
