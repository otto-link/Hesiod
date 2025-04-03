/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file string_input_diaglog.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSet>
#include <QVBoxLayout>

namespace hesiod
{

class StringInputDialog : public QDialog
{
  Q_OBJECT

public:
  StringInputDialog(const std::string              &window_title = "",
                    const std::vector<std::string> &invalid_strings = {},
                    QWidget                        *parent = nullptr);

  QString get_text() const { return input_line->text(); }

  std::string get_text_std_string() const { return input_line->text().toStdString(); }

private slots:
  void validate_input();

private:
  QLineEdit    *input_line;
  QLabel       *error_label;
  QSet<QString> invalid_strings_qset;
};

} // namespace hesiod