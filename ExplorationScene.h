#ifndef EXPLORATIONSCENE_H
#define EXPLORATIONSCENE_H
#include <QGraphicsScene>
#include <QTimer>
#include <QLabel>
#include <QPointer>
#include "scenemanager.h"
#include "BagWidget.h"
class PlayerItem;
class NPCItem;
class ExplorationScene : public QGraphicsScene {
    Q_OBJECT
  public:
    explicit ExplorationScene(SceneManager* mgr, const QString& mapName, QObject* parent = nullptr);
    ~ExplorationScene() override;
    void setPlayerPosition(const QPointF& pos);
    void setSignText(const QString& text);
    QString getMapName() const {
        return mapName;
    }
    void onSceneEntered();
    void onSceneLeft();

  protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
  private slots:
    void advance();

  private:
    SceneManager* sceneManager;
    PlayerItem* player;
    QString mapName;
    QTimer* advanceTimer;
    QTimer* npcTimer = nullptr;
    NPCItem* npc = nullptr;
    QPointer<BagWidget> bagWidget;
    QPointer<QWidget> dialogWidget;
    QPointer<QLabel> dialogLabel;
    QPointer<QLabel> aKeyIconLabel;
    QList<QPair<QRectF, QString>> signTexts;
    QStringList npcDialogPages;
    int dialogPage = 0;
    bool isNpcDialog = false;
    bool bagVisible = false;
    bool starterChosen = false;
    QList<QGraphicsPixmapItem*> starterBalls;
    QList<QRect> tallGrassAreas;
    bool isInTallGrassArea();
    void showDialogAtPlayerCenter(const QString& text);
    void initializeDialogWidget();
    void updateAKeyIcon(bool visible);
    void checkTallGrassEncounter();
};
#endif
