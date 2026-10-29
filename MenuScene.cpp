#include "MenuScene.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QFont>

MenuScene::MenuScene(SceneManager* mgr, QObject* parent)
    : QGraphicsScene(parent), sceneManager(mgr) {
    setSceneRect(0, 0, 525, 450);
    // 只顯示背景、說明
    addPixmap(QPixmap(":/new/prefix1/scene/start_menu.png"));
    QFont f("Arial", 12);
    auto help = addText("Click any where to START!!", f);
    help->setDefaultTextColor(Qt::yellow);
    help->setPos(10, 400);
}

void MenuScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);
    sceneManager->resetGame();
    sceneManager->switchTo("Town", QPointF(524, 20)); // Town 初始座標
}
