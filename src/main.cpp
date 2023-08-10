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
  tree.add_node(std::make_shared<hesiod::cnode::GammaCorrection>("gamma2"));

  tree.get_node_ref_by_id("perlin")->infos();
  tree.get_node_ref_by_id("gamma")->infos();

  ((hesiod::cnode::GammaCorrection *)tree.get_node_ref_by_id("gamma"))
      ->set_gamma(2.f);

  ((hesiod::cnode::GammaCorrection *)tree.get_node_ref_by_id("gamma2"))
      ->set_gamma(4.f);

  tree.link("perlin", "output", "gamma", "input");
  tree.link("gamma", "output", "gamma2", "input");

  hesiod::vnode::ViewTree gui_tree = hesiod::vnode::ViewTree(&tree);

  gui_tree.generate_all_view_nodes();
  gui_tree.generate_all_links();
  gui_tree.update();

  // gui_tree.view_nodes_mapping["gamma"].get()->render_settings();
  // gui_tree.view_nodes_mapping["perlin"].get()->render_settings();

  // // DBG
  // {
  //   gnode::Node     *p_node = tree.get_node_ref_by_id("perlin");
  //   hmap::HeightMap *p_h = (hmap::HeightMap *)p_node->get_p_data("output");
  //   p_h->to_array().to_png("out.png", hmap::cmap::inferno);
  // }

  tree.print_node_links();

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

    ImGui::Begin("Node editor");
    ImNodes::BeginNodeEditor();

    gui_tree.render_view_nodes();
    gui_tree.render_links();

    ImNodes::EndNodeEditor();
    ImGui::End();

    ImGui::Begin("Toto");

    for (auto &[id, vnode] : gui_tree.get_view_nodes_map())
      vnode.get()->render_settings();

    ImGui::End();

    // --- links management
    const int num_selected_links = ImNodes::NumSelectedLinks();

    if ((num_selected_links > 0) & (ImGui::IsKeyReleased(ImGuiKey_Delete) or
                                    ImGui::IsKeyReleased(ImGuiKey_X)))
    {
      std::vector<int> selected_links;
      selected_links.resize(num_selected_links);
      ImNodes::GetSelectedLinks(selected_links.data());

      for (auto &v : selected_links)
      {
        std::cout << v << "\n";
        gui_tree.remove_link(v);
      }
    }

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
