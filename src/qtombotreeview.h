#ifndef QTOMBOTREEVIEW_H
#define QTOMBOTREEVIEW_H

#include <QtCore>
#include <QtWidgets>
#include <QTreeView>


class QTomboTreeView : public QTreeView
{
public:
    QTomboTreeView();
    QTomboTreeView(QSplitter*&s);
    void mousePressEvent(QMouseEvent *event);
};

#endif // QTOMBOTREEVIEW_H
