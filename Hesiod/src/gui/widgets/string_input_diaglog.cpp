/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QDialogButtonBox>

#include "hesiod/gui/widgets/string_input_dialog.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

StringInputDialog::StringInputDialog(const std::string              &window_title,
                                     const std::vector<std::string> &invalid_strings,
                                     QWidget                        *parent)
    : QDialog(parent)
{
  setWindowTitle(window_title.c_str());

  QVBoxLayout *layout = new QVBoxLayout(this);
  input_line = new QLineEdit(this);
  error_label = new QLabel(this);
  error_label->setStyleSheet("color: red;"); // errors in red
  error_label->hide();

  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel);

  layout->addWidget(input_line);
  layout->addWidget(error_label);
  layout->addWidget(button_box);

  connect(button_box,
          &QDialogButtonBox::accepted,
          this,
          &StringInputDialog::validate_input);
  connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

  for (const auto &s : invalid_strings)
    invalid_strings_qset.insert(QString::fromStdString(s));
}

void StringInputDialog::validate_input()
{
  QString text = this->input_line->text().trimmed();

  if (this->invalid_strings_qset.contains(text))
  {
    error_label->setText("This choice is not allowed. Choose a different one.");
    error_label->show();
  }
  else
  {
    // close dialog only if input is valid
    this->accept();
  }
}

} // namespace hesiod
