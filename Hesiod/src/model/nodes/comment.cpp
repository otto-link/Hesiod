/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

// see comment_gui.cpp for GUI-specific sources

Comment::Comment(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Comment::Comment");

  // model
  this->node_caption = "Comment";

  // attributes
  this->attr["comment"] = NEW_ATTR_STRING("new comment...");

  // documentation
  this->description = "Comment node is a passive node holding a comment text.";

  this->input_descriptions = {};
  this->output_descriptions = {};

  this->attribute_descriptions["comment"] = "Text comment.";
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

} // namespace hesiod
