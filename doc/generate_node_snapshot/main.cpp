#include "ImCandy/candy.h"
#include "gnode.hpp"

#include "hesiod/viewer.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/fonts.hpp"
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

  ImCandy::Theme_Blender();

  ImGuiIO     &io = ImGui::GetIO();
  ImFontConfig config;
  config.OversampleH = 2;
  config.OversampleV = 2;
  io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf,
                                 Roboto_Regular_ttf_len,
                                 16.f,
                                 &config);

  // for (auto &[type, cat] : hesiod::cnode::category_mapping)
  // {
  //   LOG_DEBUG("type: %s", type.c_str());
  //   std::string fname = "snapshot_" + type + ".png";

  //   hesiod::vnode::ViewTree tree =
  //       hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
  //   std::string node_id = tree.add_view_node(type);

  //   export_png(window, tree, fname);
  // }

  // template

  // --- Clamp
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    auto nf = tree.add_view_node("FbmSimplex");
    auto nc = tree.add_view_node("Clamp");
    tree.new_link(nf, "output", nc, "input");

    tree.get_node_ref_by_id<hesiod::cnode::Clamp>(nc)
        ->attr.at("clamp")
        ->get_ref<hesiod::RangeAttribute>()
        ->value.x = 0.5f;

    tree.update();
    export_png(window, tree, "ex_Clamp.png");
  }

  // --- Gradient
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    auto nf = tree.add_view_node("FbmSimplex");
    auto ng = tree.add_view_node("Gradient");
    auto n1 = tree.add_view_node("Preview");
    auto n2 = tree.add_view_node("Preview");
    tree.new_link(nf, "output", ng, "input");
    tree.new_link(ng, "dx", n1, "input");
    tree.new_link(ng, "dy", n2, "input");

    tree.update();
    export_png(window, tree, "ex_Gradient.png");
  }

  // --- GradientAngle
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    auto nf = tree.add_view_node("FbmSimplex");
    auto ng = tree.add_view_node("GradientAngle");
    tree.new_link(nf, "output", ng, "input");

    tree.update();
    export_png(window, tree, "ex_GradientAngle.png");
  }

  // --- GradientNorm
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    auto nf = tree.add_view_node("FbmSimplex");
    auto ng = tree.add_view_node("GradientNorm");
    tree.new_link(nf, "output", ng, "input");

    tree.update();
    export_png(window, tree, "ex_GradientNorm.png");
  }

  // --- GradientTalus
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    auto nf = tree.add_view_node("FbmSimplex");
    auto ng = tree.add_view_node("GradientTalus");
    tree.new_link(nf, "output", ng, "input");

    tree.update();
    export_png(window, tree, "ex_GradientTalus.png");
  }

  // --- KmeansClustering2
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    tree.add_view_node("Clone");
    tree.add_view_node("GradientNorm");
    tree.add_view_node("KmeansClustering2");
    tree.add_view_node("FbmSimplex");

    tree.new_link("FbmSimplex##3", "output", "Clone##0", "input");
    tree.new_link("Clone##0", "thru##0", "GradientNorm##1", "input");
    tree.new_link("Clone##0", "thru##1", "KmeansClustering2##2", "input##2");
    tree.new_link("GradientNorm##1",
                  "output",
                  "KmeansClustering2##2",
                  "input##1");

    export_png(window, tree, "ex_KmeansClustering2.png");
  }

  //
  {
    // hesiod::vnode::ViewTree tree =
    //     hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    // tree.load_state("state.json");
    // export_png(window, tree, "out.png");
  }

  // --- Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
