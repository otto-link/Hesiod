/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QDialogButtonBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/node_info_dialog.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

NodeInfoDialog::NodeInfoDialog(BaseNode            *p_node,
                               gngui::GraphicsNode *p_gfx_node,
                               QWidget             *parent)
    : QDialog(parent)
{
  Logger::log()->trace("NodeInfoDialog::NodeInfoDialog");

  if (!p_node)
  {
    Logger::log()->error("NodeInfoDialog::NodeInfoDialog: p_node is nullptr");
    return;
  }

  if (!p_gfx_node)
  {
    Logger::log()->error("NodeInfoDialog::NodeInfoDialog: p_gfx_node is nullptr");
    return;
  }

  this->setWindowTitle("Hesiod - Node information");
  this->setMinimumWidth(256);

  QVBoxLayout *layout = new QVBoxLayout(this);

  {
    std::string str = p_node->get_caption() + "/" + p_node->get_id();
    QLabel     *label = new QLabel(str.c_str());
    layout->addWidget(label);
  }

  // --- comment

  {
    QLabel *label = new QLabel("Comment");
    layout->addWidget(label);

    QPlainTextEdit *editor = new QPlainTextEdit();
    editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    editor->setPlainText(p_node->get_comment().c_str());
    editor->setMinimumHeight(64);
    layout->addWidget(editor);

    this->connect(editor,
                  &QPlainTextEdit::textChanged,
                  this,
                  [editor, p_node, p_gfx_node]
                  {
                    if (!editor || !p_node || !p_gfx_node)
                      return;
                    p_node->set_comment(editor->toPlainText().toStdString());
                    p_gfx_node->update_proxy_widget();
                  });
  }

  // --- exit

  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok);

  this->connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);

  layout->addWidget(button_box);

  this->setLayout(layout);
}

} // namespace hesiod
