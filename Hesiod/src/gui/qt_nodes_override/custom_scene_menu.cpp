/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QTreeWidgetItem>
#include <QUndoStack>
#include <QWidgetAction>

#include "DataFlowGraphicsScene.hpp"
#include "UndoCommands.hpp"

#include "hesiod/gui/node_category_color.hpp"

namespace QtNodes
{

QMenu *DataFlowGraphicsScene::createSceneMenu(QPointF const scenePos)
{
  QMenu *modelMenu = new QMenu();

  // Add filterbox to the context menu
  auto *txtBox = new QLineEdit(modelMenu);
  txtBox->setPlaceholderText(QStringLiteral("Filter"));
  txtBox->setClearButtonEnabled(true);

  auto *txtBoxAction = new QWidgetAction(modelMenu);
  txtBoxAction->setDefaultWidget(txtBox);

  // 1.
  modelMenu->addAction(txtBoxAction);

  // Add result treeview to the context menu
  QTreeWidget *treeView = new QTreeWidget(modelMenu);
  treeView->setRootIsDecorated(false);
  treeView->setColumnCount(2);
  treeView->setHeaderLabels({"Node", "Category"});
  treeView->setSortingEnabled(true);
  treeView->sortByColumn(0, Qt::AscendingOrder);

  auto *treeViewAction = new QWidgetAction(modelMenu);
  treeViewAction->setDefaultWidget(treeView);

  // 2.
  modelMenu->addAction(treeViewAction);

  auto registry = _graphModel.dataModelRegistry();

  QFont font = treeView->font();
  font.setPointSize(font.pointSize() - 2);
  treeView->setFont(font);

  for (auto const &assoc : registry->registeredModelsCategoryAssociation())
  {
    auto item = new QTreeWidgetItem(treeView);
    item->setText(0, assoc.first);
    item->setText(1, assoc.second);

    // category color
    std::string node_category = assoc.second.toStdString();
    int         pos = node_category.find("/");
    std::string main_category = node_category.substr(0, pos);

    QColor cat_color = QColor(255, 0, 0);
    if (hesiod::node_category_color.contains(main_category))
      cat_color = hesiod::node_category_color.at(main_category);

    item->setForeground(1, cat_color);
  }

  treeView->expandAll();

  connect(treeView,
          &QTreeWidget::itemClicked,
          [this, modelMenu, scenePos](QTreeWidgetItem *item, int)
          {
            if (!(item->flags() & (Qt::ItemIsSelectable)))
            {
              return;
            }

            this->undoStack().push(new CreateCommand(this, item->text(0), scenePos));

            modelMenu->close();
          });

  // Setup filtering
  connect(txtBox,
          &QLineEdit::textChanged,
          [treeView](const QString &text)
          {
            QTreeWidgetItemIterator categoryIt(treeView,
                                               QTreeWidgetItemIterator::HasChildren);
            while (*categoryIt)
              (*categoryIt++)->setHidden(true);
            QTreeWidgetItemIterator it(treeView, QTreeWidgetItemIterator::NoChildren);
            while (*it)
            {
              auto       modelName = (*it)->text(0);
              const bool match = (modelName.contains(text, Qt::CaseInsensitive));
              (*it)->setHidden(!match);
              if (match)
              {
                QTreeWidgetItem *parent = (*it)->parent();
                while (parent)
                {
                  parent->setHidden(false);
                  parent = parent->parent();
                }
              }
              ++it;
            }
          });

  // make sure the text box gets focus so the user doesn't have to click on it
  txtBox->setFocus();

  // QMenu's instance auto-destruction
  modelMenu->setAttribute(Qt::WA_DeleteOnClose);

  return modelMenu;
}

} // namespace QtNodes
