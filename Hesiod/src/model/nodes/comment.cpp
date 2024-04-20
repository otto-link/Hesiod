/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Comment::Comment(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Comment::Comment");

  // model
  this->node_caption = "Comment";

  // attributes
  this->attr["comment"] = NEW_ATTR_STRING("new comment...");
}

std::shared_ptr<QtNodes::NodeData> Comment::outData(QtNodes::PortIndex /* port_index */)
{
  return std::shared_ptr<QtNodes::NodeData>();
}

void Comment::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                        QtNodes::PortIndex /* port_index */)
{
  // empty
}

// --- computing

void Comment::compute()
{
  Q_EMIT this->computingStarted();

  // empty

  Q_EMIT this->computingFinished();
}

// --- GUI

QWidget *Comment::embeddedWidget()
{
  if (!this->label_comment)
  {
    this->label_comment = new QLabel();
    this->label_comment->setMinimumSize(128, 128);
    this->label_comment->setMaximumSize(128, 128);
    this->label_comment->setStyleSheet("background-color: #3B3B3B; color: #DFE1E2");
    this->label_comment->setAlignment(Qt::AlignTop);
    this->label_comment->setWordWrap(true);

    QFont font = this->label_comment->font();
    font.setPointSize(font.pointSize() - 3);
    font.setItalic(true);
    this->label_comment->setFont(font);

    this->label_comment->setText(GET_ATTR_STRING("comment").c_str());

    connect(this,
            &BaseNode::node_loaded,
            [this]()
            { this->label_comment->setText(GET_ATTR_STRING("comment").c_str()); });

    connect(this,
            &BaseNode::settings_changed,
            [this]()
            { this->label_comment->setText(GET_ATTR_STRING("comment").c_str()); });
  }
  return (QWidget *)this->label_comment;
}

} // namespace hesiod
