#include "hesiod/control_tree.hpp"
#include "hesiod/view_tree.hpp"
#include "highmap/geometry.hpp"
#define _USE_MATH_DEFINES
typedef unsigned int uint;

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "hesiod/gui.hpp"
#include "hesiod/window.hpp"
#include "macrologger.h"
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

  hesiod::window::WindowImplemented *window =
      new hesiod::window::WindowImplemented("Hesiod", 1600, 900);

  if (window->initialize() == false)
  {
    LOG_ERROR("initialize failed.");
    return -1;
  }

  hesiod::vnode::ViewTree *tree =
      new hesiod::vnode::ViewTree("tree_1", shape, tiling, overlap);

  tree->add_view_node("NoiseFbm");
  tree->add_view_node("Noise");

  window->get_window_manager()->add_window(tree);

  window->run();

  if (window->shutdown() == false)
  {
    LOG_ERROR("shutdown failed.");
    return -1;
  }

  std::cout << "ok" << std::endl;
  return 0;
}
