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

  hesiod::vnode::ViewTree tree =
      hesiod::vnode::ViewTree("tree_1", shape, tiling, overlap);

  // tree.generate_all_links();
  tree.update();

  tree.print_node_links();

  // ----------------------------------- Main GUI

  GLFWwindow *window =
      hesiod::gui::init_gui(1400, 800, "Hesiod v0.0.x (c) 2023 Otto Link");
  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

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

    tree.render_view_nodes();
    tree.render_links();

    // if (tree.size() > 1)
    // {
    //   std::vector<gnode::Point> positions = tree.compute_graph_layout();

    //   size_t k = 0;
    //   for (auto &[id, vnode] : tree.get_nodes_map())
    //   {
    //     LOG_DEBUG("%s %d", id.c_str(), vnode.get()->hash_id);
    //     // ImVec2 npos = ImNodes::GetNodeGridSpacePos(vnode.get()->hash_id);
    //     ImNodes::SetNodeGridSpacePos(vnode.get()->hash_id,
    //                                  {300 * positions[k].x, 300 * positions[k].y});
    //     k++;
    //     // LOG_DEBUG("%f %f", npos.x, npos.y);
    //   }
    // }

    ImNodes::EndNodeEditor();
    ImGui::End();

    // --- links management // new
    int port_hash_id_from, port_hash_id_to;
    if (ImNodes::IsLinkCreated(&port_hash_id_from, &port_hash_id_to))
    {
      LOG_DEBUG("link created: %d <-> %d", port_hash_id_from, port_hash_id_to);
      tree.new_link(port_hash_id_from, port_hash_id_to);
    }

    // --- links management // destruction
    const int num_selected_links = ImNodes::NumSelectedLinks();

    if ((num_selected_links > 0) & (ImGui::IsKeyReleased(ImGuiKey_Delete) or
                                    ImGui::IsKeyReleased(ImGuiKey_X)))
    {
      std::vector<int> selected_links;
      selected_links.resize(num_selected_links);
      ImNodes::GetSelectedLinks(selected_links.data());

      for (auto &v : selected_links)
        tree.remove_link(v);
    }

    // --- nodes // new
    ImGui::Begin("New node");
    tree.render_new_node_treeview();
    ImGui::End();

    // --- nodes DEBUG node list
    ImGui::Begin("Node list");
    tree.render_node_list();
    ImGui::End();

    // --- nodes // settings
    const int num_selected_nodes = ImNodes::NumSelectedNodes();

    ImGui::Begin("Settings");
    if (num_selected_nodes > 0)
    {
      std::vector<int> selected_nodes;
      selected_nodes.resize(num_selected_nodes);
      ImNodes::GetSelectedNodes(selected_nodes.data());

      std::vector<std::string> node_id_to_remove = {};

      for (auto &node_hash_id : selected_nodes)
      {
        std::string node_id = tree.get_node_id_by_hash_id(node_hash_id);
        ImGui::SeparatorText(node_id.c_str());
        tree.render_settings(node_id);

        // delete node
        if (ImGui::IsKeyReleased(ImGuiKey_Delete) or
            ImGui::IsKeyReleased(ImGuiKey_X))
          node_id_to_remove.push_back(node_id);

        // duplicate node
        if (ImGui::IsKeyReleased(ImGuiKey_D))
          tree.add_view_node(tree.get_node_ref_by_id(node_id)->get_node_type());
      }

      for (auto &node_id : node_id_to_remove)
      {
        tree.remove_view_node(node_id);
        ImNodes::ClearNodeSelection();
      }
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
