#ifndef TSUTREEWIDGET_H
#define TSUTREEWIDGET_H

#include <QObject>
#include <QtGui>
#include <QTreeWidget>

class tsuTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit tsuTreeWidget(QWidget* parent);
    virtual ~tsuTreeWidget();

public slots:
  void showDirectory(QTreeWidgetItem *item, int column);
  void expandDirectory(QTreeWidgetItem *item);

private:
  QTreeWidgetItem *p_headerItem{};

  void loadDriveStructure(const QString& drive);
  void addChildren(QTreeWidgetItem* item, const QString& filePath);
};

#endif // TSUTREEWIDGET_H
