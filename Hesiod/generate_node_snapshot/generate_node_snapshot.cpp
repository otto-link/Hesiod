#include "generate_node_snapshot.hpp"

#include "gnode.hpp"

#include "hesiod/viewer.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/fonts.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

#if ENABLE_GENERATE_NODE_SNAPSHOT
#define SNAPSHOT_SIZE 512

static void export_png(GLFWwindow              *window,
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

void generate_node_snapshot()
{
  hmap::Vec2<int> shape = {512, 512};
  hmap::Vec2<int> tiling = {1, 1};
  float           overlap = 0.f;

  GLFWwindow *window = hesiod::gui::init_gui(
      2 * SNAPSHOT_SIZE,
      SNAPSHOT_SIZE,
      "Hesiod v0.0.x (c) 2023 Otto Link");
  
  // What the hell is going on in ImCandy?
  // ImCandy::Theme_Blender();

  ImGuiIO     &io = ImGui::GetIO();
  ImFontConfig config;
  config.OversampleH = 2;
  config.OversampleV = 2;
  config.FontDataOwnedByAtlas = false;
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

    tree.set_viewer_node_id(nc);
    tree.export_view3d("ex_Clamp_render.png");
  }

  // --- Cloud / CloudToArrayInterp
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    auto nc = tree.add_view_node("Cloud");
    auto nf = tree.add_view_node("FbmSimplex");
    auto na = tree.add_view_node("CloudToArrayInterp");
    tree.new_link(nf, "output", na, "dx");
    tree.new_link(nc, "output", na, "cloud");

    hmap::Cloud value = hmap::Cloud({0.1f, 0.7f, 0.5f, 0.8f},
                                    {0.2f, 0.5f, 0.8f, 0.1f},
                                    {0.1f, 0.2f, 1.f, 0.5f});

    tree.get_node_ref_by_id<hesiod::cnode::Cloud>(nc)
        ->attr.at("cloud")
        ->get_ref<hesiod::CloudAttribute>()
        ->value = value;

    tree.update();
    export_png(window, tree, "ex_Cloud.png");
    export_png(window, tree, "ex_CloudToArrayInterp.png");

    tree.set_viewer_node_id(na);
    tree.export_view3d("ex_CloudToArrayInterp_render.png");
  }

  // --- FbmPerlin
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    auto nf = tree.add_view_node("FbmPerlin");

    tree.update();
    export_png(window, tree, "ex_FbmPerlin.png");

    tree.set_viewer_node_id(nf);
    tree.export_view3d("ex_FbmPerlin_render.png");
  }

  // --- FbmSimplex
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    auto nf = tree.add_view_node("FbmSimplex");

    tree.update();
    export_png(window, tree, "ex_FbmSimplex.png");

    tree.set_viewer_node_id(nf);
    tree.export_view3d("ex_FbmSimplex_render.png");
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

    tree.set_viewer_node_id(ng);
    tree.export_view3d("ex_GradientAngle_render.png");
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

    tree.set_viewer_node_id(ng);
    tree.export_view3d("ex_GradientNorm_render.png");
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

    tree.set_viewer_node_id(ng);
    tree.export_view3d("ex_GradientTalus_render.png");
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

  // --- MakeBinary
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);
    auto nf = tree.add_view_node("FbmSimplex");
    auto nc = tree.add_view_node("MakeBinary");
    tree.new_link(nf, "output", nc, "input");

    tree.get_node_ref_by_id<hesiod::cnode::MakeBinary>(nc)
        ->attr.at("threshold")
        ->get_ref<hesiod::FloatAttribute>()
        ->value = 0.5f;

    tree.update();
    export_png(window, tree, "ex_MakeBinary.png");

    tree.set_viewer_node_id(nc);
    tree.export_view3d("ex_MakeBinary_render.png");
  }

  // --- WaveSine
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    auto nf = tree.add_view_node("WaveSine");
    auto nd = tree.add_view_node("FbmSimplex");
    tree.new_link(nd, "output", nf, "dx");

    tree.get_node_ref_by_id<hesiod::cnode::FbmSimplex>(nd)
        ->attr.at("remap")
        ->get_ref<hesiod::RangeAttribute>()
        ->value.y = 0.2f;

    tree.update();
    export_png(window, tree, "ex_WaveSine.png");

    tree.set_viewer_node_id(nf);
    tree.export_view3d("ex_WaveSine_render.png");
  }

  // --- WaveSquare
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    auto nf = tree.add_view_node("WaveSquare");
    auto nd = tree.add_view_node("FbmSimplex");
    tree.new_link(nd, "output", nf, "dx");

    tree.get_node_ref_by_id<hesiod::cnode::FbmSimplex>(nd)
        ->attr.at("remap")
        ->get_ref<hesiod::RangeAttribute>()
        ->value.y = 0.2f;

    tree.update();
    export_png(window, tree, "ex_WaveSquare.png");

    tree.set_viewer_node_id(nf);
    tree.export_view3d("ex_WaveSquare_render.png");
  }

  // --- WaveTriangular
  {
    hesiod::vnode::ViewTree tree =
        hesiod::vnode::ViewTree("tree", shape, tiling, overlap);

    auto nf = tree.add_view_node("WaveTriangular");
    auto nd = tree.add_view_node("FbmSimplex");
    tree.new_link(nd, "output", nf, "dx");

    tree.get_node_ref_by_id<hesiod::cnode::FbmSimplex>(nd)
        ->attr.at("remap")
        ->get_ref<hesiod::RangeAttribute>()
        ->value.y = 0.2f;

    tree.update();
    export_png(window, tree, "ex_WaveTriangular.png");

    tree.set_viewer_node_id(nf);
    tree.export_view3d("ex_WaveTriangular_render.png");
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

  return;
}

#endif