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

// in this order, required by args.hxx
std::istream &operator>>(std::istream &is, hmap::Vec2<int> &vec2)
{
  is >> vec2.x;
  is.get();
  is >> vec2.y;
  return is;
}
#include <args.hxx>

int main(int argc, char *argv[])
{
  // --- parse command line arguments

  args::ArgumentParser parser("Hesiod.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

  args::ValueFlag<std::string> batch(parser,
                                     "batch mode",
                                     "Execute ...",
                                     {'b', "batch"});

  args::ValueFlag<hmap::Vec2<int>> shape_arg(
      parser,
      "shape",
      "Heightmap shape (in pixels), ex. --shape=512,512",
      {"shape"});

  args::ValueFlag<hmap::Vec2<int>> tiling_arg(
      parser,
      "tiling",
      "Heightmap tiling, ex. --tiling=4,4",
      {"tiling"});

  args::ValueFlag<float> overlap_arg(
      parser,
      "overlap",
      "Tile overlapping ratio (in [0, 1[), ex. --overlap=0.25",
      {"overlap"});

  try
  {
    parser.ParseCLI(argc, argv);

    // batch mode
    if (batch)
    {
      LOG_INFO("executing Hesiod in batch mode...");
      LOG_DEBUG("file: %s", args::get(batch).c_str());

      hmap::Vec2<int> shape = shape_arg == true ? args::get(shape_arg)
                                                : hmap::Vec2<int>(0, 0);
      hmap::Vec2<int> tiling = tiling_arg == true ? args::get(tiling_arg)
                                                  : hmap::Vec2<int>(0, 0);
      float overlap = overlap_arg == true ? args::get(overlap_arg) : -1.f;

      LOG_DEBUG("cli shape: {%d, %d}", shape.x, shape.y);
      LOG_DEBUG("cli tiling: {%d, %d}", tiling.x, tiling.y);
      LOG_DEBUG("cli overlap: %f", overlap);

      hesiod::cnode::ControlTree c_tree = hesiod::cnode::ControlTree("batch");
      c_tree.load_state(args::get(batch).c_str(), shape, tiling, overlap);

      return 0;
    }
  }
  catch (args::Help)
  {
    std::cout << parser;
    return 0;
  }

  catch (args::Error &e)
  {
    std::cerr << e.what() << std::endl << parser;
    return 1;
  }

  // ----------------------------------- Main GUI

  hmap::Vec2<int> shape = {1024, 1024};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.25f;

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

  tree.add_view_node("NoiseFbm");
  tree.add_view_node("Noise");

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
