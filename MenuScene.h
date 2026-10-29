#ifndef MENUSCENE_H
#define MENUSCENE_H

#include <QGraphicsScene>
#include <QKeyEvent>
#include "scenemanager.h"

class MenuScene : public QGraphicsScene {
    Q_OBJECT
  public:
    explicit MenuScene(SceneManager* mgr, QObject* parent = nullptr);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

  protected:
    // void keyPressEvent(QKeyEvent* event) override;

  private:
    SceneManager* sceneManager;
};

#endif // MENUSCENE_H
