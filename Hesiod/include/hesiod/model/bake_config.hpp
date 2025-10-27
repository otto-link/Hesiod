/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
 * License. The full license is in the file LICENSE, distributed with this software. */
#pragma once

namespace hesiod
{

// =====================================
// BakeConfig
// =====================================
struct BakeConfig
{
  int  resolution = 1024;
  int  nvariants = 0;
  bool force_distributed = true;
  bool force_auto_export = true;
  bool rename_export_files = true;
};

} // namespace hesiod