// --- PlayerItem.h ---
#ifndef PLAYERITEM_H
#define PLAYERITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPointF>

class PlayerItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
  public:
    enum Direction { Up = 0, Down = 1, Left = 2, Right = 3, NoneDir = 4 };

    explicit PlayerItem(QGraphicsItem* parent = nullptr);

    void setDirection(Direction dir);

  protected:
    void advance(int phase) override;

  private:
    Direction currentDir = NoneDir;
    int frameIndex = 0;
    static constexpr qreal speed = 2.0;
    QVector<QPixmap> frames[4];
    void loadFrames();
};

#endif // PLAYERITEM_H
