// barriereditor.h - 改為探索場景中繪製障礙物的工具類別
#ifndef BARRIEREDITOR_H
#define BARRIEREDITOR_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QList>
#include <QRect>

class BarrierOverlay {
  public:
    BarrierOverlay(QGraphicsScene* scene);
    void drawBarriers(const QString& mapName);

  private:
    QGraphicsScene* scene;
};

#endif // BARRIEREDITOR_H
