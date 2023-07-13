#include <iostream>

#include "highmap/array.hpp"
#include "highmap/primitives.hpp"

int main()
{
  hmap::Array array = hmap::constant({8, 8}, 1.f);
  array.infos();
  
  std::cout << "ok" << std::endl;
  return 0;
}
