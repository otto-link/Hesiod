/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QFormLayout>
#include <QObject>
#include <QWidget>

namespace hesiod
{

class AppSettingsWindow : public QWidget
{
  Q_OBJECT
public:
  explicit AppSettingsWindow(QWidget *parent = nullptr);

private:
  void setup_layout();

  void add_description(const std::string &description, int max_length = 64);
  void add_title(const std::string &label, int font_size_delta = 2);
  void bind_bool(const std::string &label, bool &state);

  QFormLayout *layout;
};

} // namespace hesiod