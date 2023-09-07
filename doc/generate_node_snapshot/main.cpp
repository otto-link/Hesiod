#include "gnode.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

#define SNAPSHOT_SIZE 512

void export_png(GLFWwindow              *window,
                hesiod::vnode::ViewTree &tree,
                std::string              fname)
{
  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  for (int i = 0; i < 30; i++)
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(
        ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(2 * SNAPSHOT_SIZE, SNAPSHOT_SIZE));

    tree.render_node_editor();

    ax::NodeEditor::SetCurrentEditor(tree.get_p_node_editor_context());
    tree.automatic_node_layout();
    ax::NodeEditor::NavigateToContent(0.f);
    ax::NodeEditor::SetCurrentEditor(nullptr);

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
  hesiod::gui::save_screenshot(fname);
}

int main()
{
  hmap::Vec2<int> shape = {512, 512};
  hmap::Vec2<int> tiling = {1, 1};
  float           overlap = 0.f;

  GLFWwindow *window = hesiod::gui::init_gui(
      2 * SNAPSHOT_SIZE,
      SNAPSHOT_SIZE,
      "Hesiod v0.0.x (c) 2023 Otto Link");

  for (auto &[type, cat] : hesiod::cnode::category_mapping)
  {
    LOG_DEBUG("type: %s", type.c_str());
    std::string fname = "snapshot_" + type + ".png";

    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    tree.add_view_node(type);

    export_png(window, tree, fname);
  }

  // template

  // --- Clamp
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    tree.add_view_node("FbmPerlin");
    tree.add_view_node("Clamp");
    tree.new_link("FbmPerlin##0", "output", "Clamp##1", "input");
    export_png(window, tree, "ex_Clamp.png");
  }

  // --- KmeansClustering2
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    tree.add_view_node("Clone");
    tree.add_view_node("GradientNorm");
    tree.add_view_node("KmeansClustering2");
    tree.add_view_node("FbmPerlin");
    tree.print_node_list();
    tree.new_link("FbmPerlin##3", "output", "Clone##0", "input");
    tree.new_link("Clone##0", "thru##0", "GradientNorm##1", "input");
    tree.new_link("Clone##0", "thru##1", "KmeansClustering2##2", "input##2");
    tree.new_link("GradientNorm##1",
                  "output",
                  "KmeansClustering2##2",
                  "input##1");
    export_png(window, tree, "ex_KmeansClustering2.png");
  }

  // --- Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
