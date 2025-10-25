/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#include "nlohmann/json.hpp"

#include "hesiod/app/app_context.hpp"

#define HSD_CTX                                                                          \
  static_cast<hesiod::HesiodApplication *>(QCoreApplication::instance())->get_context()

namespace hesiod
{

class HesiodApplication : public QApplication
{
  Q_OBJECT
public:
  HesiodApplication(int &argc, char **argv);
  ~HesiodApplication();

  QApplication     &get_qapp();
  AppContext       &get_context();
  const AppContext &get_context() const;

private:
  void load_settings();
  void save_settings() const;

  // --- Members
  AppContext context;
};

// helpers

std::string get_config_file_path(const QString &app_name, bool portable_mode);
std::string get_config_file_path_auto(const QString &app_name);

} // namespace hesiod