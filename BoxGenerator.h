#ifndef BOXGENERATOR_H
#define BOXGENERATOR_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPoint>
#include <vector>

class BoxGenerator {
  public:
    BoxGenerator(QGraphicsScene* scene);
    void generateBoxes(int count); // 隨機產生 count 個箱子

  private:
    QGraphicsScene* scene;
    std::vector<QPoint> itemPositions;
};

#endif // BOXGENERATOR_H
