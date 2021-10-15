#include "qtombotreeview.h"

QTomboTreeView::QTomboTreeView()
{
}

QTomboTreeView::QTomboTreeView(QSplitter*&s)
{
}

 void QTomboTreeView::mousePressEvent(QMouseEvent *event)
 {
     if (event->button() == Qt::RightButton) {
         // handle left mouse button here
     } else {
         // pass on other buttons to base class
         QTreeView::mousePressEvent(event);
     }
 }
