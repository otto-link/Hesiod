/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;

#include "hesiod/logger.hpp"

int main() // (int argc, char *argv[])
{
  SPDLOG->info("Welcome to Hesiod v{}.{}.{}!",
               HESIOD_VERSION_MAJOR,
               HESIOD_VERSION_MINOR,
               HESIOD_VERSION_PATCH);

  return 0;
}
