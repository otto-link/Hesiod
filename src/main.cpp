#include <iostream>

#include "macrologger.h"

#include "gnode.hpp"
#include "highmap.hpp"

int main()
{
  hmap::Array array = hmap::constant(hmap::Vec2(8, 8), 1.f);
  array.infos();

  std::cout << "ok" << std::endl;
  return 0;
}
