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

  gnode::Tree tree = gnode::Tree("tree_1");

  tree.add_node(std::make_shared<hesiod::cnode::Perlin>("perlin",
                                                        shape,
                                                        tiling,
                                                        overlap));

  tree.add_node(std::make_shared<hesiod::cnode::GammaCorrection>("gamma"));

  tree.get_node_ref_by_id("perlin")->infos();
  tree.get_node_ref_by_id("gamma")->infos();

  ((hesiod::cnode::GammaCorrection *)tree.get_node_ref_by_id("gamma"))
      ->set_gamma(4.f);

  tree.link("perlin", "output", "gamma", "input");

  // tree.update_node("perlin");

  // GUI
  // hesiod::vnode::ViewPerlin gui_perlin = hesiod::vnode::ViewPerlin(
  //     (hesiod::cnode::Perlin *)tree.get_node_ref_by_id("perlin"));

  // hesiod::vnode::ViewGammaCorrection gui_gamma =
  //     hesiod::vnode::ViewGammaCorrection(
  //         (hesiod::cnode::GammaCorrection
  //         *)tree.get_node_ref_by_id("gamma"));

  // gui_perlin.render_settings();
  // gui_gamma.render_settings();

  hesiod::vnode::ViewTree gui_tree = hesiod::vnode::ViewTree(&tree);

  gui_tree.update();
  gui_tree.generate_all_view_nodes();

  // gui_tree.view_nodes_mapping["gamma"].get()->render_settings();
  // gui_tree.view_nodes_mapping["perlin"].get()->render_settings();

  hesiod::vnode::Link link =
      hesiod::vnode::Link("perlin", "output", "gamma", "input");

  // DBG
  {
    gnode::Node     *p_node = tree.get_node_ref_by_id("perlin");
    hmap::HeightMap *p_h = (hmap::HeightMap *)p_node->get_p_data("output");
    p_h->to_array().to_png("out.png", hmap::cmap::inferno);
  }

  // ----------------------------------- Main GUI

  GLFWwindow *window = init_gui(1400, 800, "Hesiod v0.0.x (c) 2023 Otto Link");
  ImVec4      clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  ImNodes::CreateContext();
  ImNodes::StyleColorsDark();
  // ImNodes::StyleColorsClassic();

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- GUI content

    ImGui::Begin("Toto");

    if (gui_tree.view_nodes_mapping["perlin"].get()->render_settings())
    {
      LOG_DEBUG("has changed");
      gnode::Node     *p_node = tree.get_node_ref_by_id("perlin");
      hmap::HeightMap *p_h = (hmap::HeightMap *)p_node->get_p_data("output");
      p_h->to_array().to_png("out.png", hmap::cmap::inferno);
    }

    ImGui::End();

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
