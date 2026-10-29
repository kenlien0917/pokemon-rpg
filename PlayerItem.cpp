#include "PlayerItem.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>

PlayerItem::PlayerItem(QGraphicsItem* parent) : QObject(), QGraphicsPixmapItem(parent) {
    loadFrames();
    currentDir = NoneDir;
    frameIndex = 0;
    setZValue(20);

    // Movement is controlled by collision-checked keyboard input.
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption); // 或 try ItemIgnoresTransformations, false

    if (!frames[Down].isEmpty()) {
        setPixmap(frames[Down][0]);
    } else {
        qWarning() << "Player frame images failed to load.";
    }
}

void PlayerItem::setDirection(Direction dir) {
    currentDir = dir;
}

void PlayerItem::advance(int phase) {
    if (phase == 0 || !isEnabled() || !scene())
        return;
    if (currentDir == NoneDir)
        return;

    // 預測下一幀位置
    QPointF nextPos = pos();
    switch (currentDir) {
    case Up:
        nextPos.ry() -= speed;
        break;
    case Down:
        nextPos.ry() += speed;
        break;
    case Left:
        nextPos.rx() -= speed;
        break;
    case Right:
        nextPos.rx() += speed;
        break;
    default:
        break;
    }

    // 計算下一幀中心點
    QPointF center = nextPos + QPointF(boundingRect().width() / 2, boundingRect().height() / 2);

    // 碰撞預檢
    bool blocked = false;
    // Maps, portals and interaction radii are authored around the sprite center.
    if (!scene()->sceneRect().contains(center))
        return;
    for (auto* item : scene()->items(center)) {
        QString type = item->data(0).toString();
        if ((type == "barrier") || (type == "ledge" && currentDir == Up)) {
            if (item->contains(item->mapFromScene(center))) {
                blocked = true;
                break;
            }
        }
    }

    if (blocked)
        return;

    // 正式移動
    setPos(nextPos);

    // 動畫更新
    if (frames[currentDir].size() >= 3) {
        frameIndex = (frameIndex + 1) % 2;
        setPixmap(frames[currentDir][frameIndex + 1]);
    } else if (!frames[currentDir].isEmpty()) {
        setPixmap(frames[currentDir][0]);
    }
}

void PlayerItem::loadFrames() {
    QStringList directions = {"B", "F", "L", "R"};
    for (int d = 0; d < 4; ++d) {
        frames[d].clear();

        // 先載入靜止圖（站立）
        QString stillPath = QString(":/new/prefix1/player/player_%1.png").arg(directions[d]);
        QPixmap stillPixmap(stillPath);
        if (!stillPixmap.isNull()) {
            frames[d].append(stillPixmap);
        } else {
            qWarning() << "Failed to load" << stillPath;
        }

        // 再載入移動圖（兩幀）
        for (int f = 1; f <= 2; ++f) {
            QString walkPath =
                QString(":/new/prefix1/player/player_%1W%2.png").arg(directions[d]).arg(f);
            QPixmap walkPixmap(walkPath);
            if (!walkPixmap.isNull()) {
                frames[d].append(walkPixmap);
            } else {
                qWarning() << "Failed to load" << walkPath;
            }
        }
    }
}
