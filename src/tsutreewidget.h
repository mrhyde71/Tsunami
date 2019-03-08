#ifndef TSUTREEWIDGET_H
#define TSUTREEWIDGET_H

#include <QObject>
#include <QtGui>
#include <QTreeWidget>

class tsuTreeWidget : public QTreeWidget
{
public:
    explicit tsuTreeWidget(QWidget* parent);
    ~tsuTreeWidget();

public slots:
  void showDirectory(QTreeWidgetItem *item, int column);
  void expandDirectory(QTreeWidgetItem *item);

private:
  QTreeWidgetItem *p_headerItem;

  void loadDriveStructure(const QString& drive);
  void addChildren(QTreeWidgetItem* item, const QString& filePath);

  QString convertSize(const uint64_t &size);
  QString convertSizeUnit(const uint64_t &size);
};

#endif // TSUTREEWIDGET_H
