#ifndef PTABLEWIDGET_H
#define PTABLEWIDGET_H

#include <QTableWidget>

class QMenu;

class pTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit pTableWidget(QWidget *parent = 0);

    void setDefaultSortedColumn(int column) { m_defaultSortedColumn = column; }
    int findText(const QString &text, int column);
    void sortByColumn(int column, Qt::SortOrder order);
    void sortByColumn(int column);

public slots:
    int addRow();
    void removeRow(int row);
    void removeAll();
    virtual void showContextMenu(const QPoint &pos);
    void setCurrentRow(int row);

    void updateSelectedRowFromToolButton();
    void updateToolButtonNumber();

private:
    int m_defaultSortedColumn;

};

#endif // PTABLEWIDGET_H
