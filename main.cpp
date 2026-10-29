#include <QApplication>
#include <QGraphicsView>
#include "scenemanager.h"
#include "MenuScene.h"
#include "ExplorationScene.h"
#include "BattleScene.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QGraphicsView view;
    view.setFixedSize(525, 450);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    SceneManager mgr(&view);
    mgr.registerScene("Menu", new MenuScene(&mgr));
    mgr.registerScene("Grassland", new ExplorationScene(&mgr, "GrassLand"));
    mgr.registerScene("Town", new ExplorationScene(&mgr, "Town"));
    mgr.registerScene("lab", new ExplorationScene(&mgr, "lab"));
    mgr.registerScene("Battle", new BattleScene(&mgr));
    mgr.switchTo("Menu");
    view.show();

    return a.exec();
}
