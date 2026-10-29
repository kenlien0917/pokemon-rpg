// --- ExplorationScene.cpp ---
#include "ExplorationScene.h"
#include "PlayerItem.h"
#include "BattleScene.h"
#include "MenuScene.h"
#include "scenemanager.h"
#include "BarrierEditor.h"
#include "BoxGenerator.h"
#include <QGraphicsView>
#include <QKeyEvent>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QDebug>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QLabel>

class NPCItem : public QGraphicsPixmapItem {
  public:
    NPCItem() {
        setPixmap(QPixmap(":/new/prefix1/NPC.png"));
        setZValue(19);
        setData(0, "barrier");
        setData(1, "sign");
    }

    void wander(QGraphicsScene* scene) {
        int dx = QRandomGenerator::global()->bounded(-1, 2);
        int dy = QRandomGenerator::global()->bounded(-1, 2);
        QPointF next = pos() + QPointF(dx * 2, dy * 2);
        setPos(next);

        auto colliders = scene->collidingItems(this);
        for (auto* it : colliders) {
            QString type = it->data(0).toString();
            if (type == "barrier" || type == "ledge") {
                setPos(pos() - QPointF(dx * 2, dy * 2));
                break;
            }
        }
    }
};

ExplorationScene::ExplorationScene(SceneManager* mgr, const QString& mapName, QObject* parent)
    : QGraphicsScene(parent), sceneManager(mgr), mapName(mapName) {
    setItemIndexMethod(NoIndex);
    QString path = QString(":/new/prefix1/scene/%1.png").arg(mapName);
    QPixmap bg(path);
    if (bg.isNull()) {
        qWarning() << "背景載入失敗:" << path;
        bg = QPixmap(1000, 1000);
        bg.fill(Qt::darkGray);
    }

    setSceneRect(0, 0, bg.width(), bg.height());
    auto bgItem = addPixmap(bg);
    bgItem->setZValue(-1);

    player = new PlayerItem;
    player->setZValue(20);
    addItem(player);
    BarrierOverlay overlay(this);
    overlay.drawBarriers(mapName);
    if (mapName == "lab") {
        player->setPos(QPointF(500, 670));
        npc = new NPCItem;
        npc->setPos(580, 610);
        addItem(npc);

        npcDialogPages =
            QStringList()
            << "I am Professor Oak. Welcome to my laboratory!"
            << "You can choose one from three Poké Balls as your initial\n Pokémon in Laboratory.";

        signTexts.append({QRectF(580, 610, 40, 60), npcDialogPages[0]});

        npcTimer = new QTimer(this);
        connect(npcTimer, &QTimer::timeout, this, [=]() {
            if (npc)
                npc->wander(this);
        });
        npcTimer->setInterval(500);

        if (!starterChosen) {
            QPixmap ballPixmap(":/new/prefix1/ball.png");
            if (ballPixmap.isNull()) {
                qWarning() << "Starter Pokéball image failed to load!";
            }
            QList<QPointF> ballPositions = {{580, 420}, {620, 420}, {660, 420}};
            QList<QString> pokemonNames = {"Squirtle", "Charmander", "Bulbasaur"};

            for (int i = 0; i < ballPositions.size(); ++i) {
                QGraphicsPixmapItem* ball = addPixmap(ballPixmap);
                ball->setPos(ballPositions[i]);
                ball->setZValue(5);
                ball->setData(0, "starter_ball");
                ball->setData(1, pokemonNames[i]);
                starterBalls.append(ball); // 儲存指標以便後續清除
            }
        }

    } else if (mapName == "GrassLand") {
        player->setPos(QPointF(524, 1615));

        tallGrassAreas = {{420, 250, 490, 200}, {675, 550, 230, 200},  {500, 1000, 250, 200},
                          {90, 1330, 375, 170}, {630, 1330, 275, 170}, {510, 1465, 75, 200}};
        QPen redPen(Qt::NoPen);
        redPen.setWidth(2);
        for (const QRect& rect : tallGrassAreas) {
            addRect(rect, redPen)->setZValue(100); // 顯示在最上層
        }
    } else if (mapName == "Town") {
        player->setPos(QPointF(524, 20));
        BoxGenerator boxGen(this);
        boxGen.generateBoxes(15);
    } else {
        player->setPos(sceneRect().center());
    }

    advanceTimer = new QTimer(this);
    advanceTimer->setInterval(16);
    connect(advanceTimer, &QTimer::timeout, this, &ExplorationScene::advance);

    dialogWidget = nullptr;
    dialogLabel = nullptr;

    if (mapName == "Town") {
        signTexts.append({QRectF(195, 670, 70, 75), "Bring some flowers with you."});
        signTexts.append({QRectF(375, 524, 60, 58), "This is Pallet Town. Begin your adventure!"});
        signTexts.append({QRectF(670, 780, 60, 55), "This is the Lab."});
    } else if (mapName == "GrassLand") {
        signTexts.append({QRectF(355, 1265, 80, 55), "This is GrassLand, Home of Pokemon."});
    }

    bagWidget = new BagWidget();
    bagWidget->setParent(nullptr); // 獨立視窗
    bagWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    bagWidget->setAttribute(Qt::WA_TranslucentBackground);
    bagWidget->hide();
}

bool ExplorationScene::isInTallGrassArea() {
    QRectF playerRect = player->boundingRect().translated(player->pos());

    for (const auto& grassRect : tallGrassAreas) {
        if (playerRect.intersects(grassRect)) {
            return true;
        }
    }
    return false;
}

void ExplorationScene::advance() {
    if (views().isEmpty() || views().first()->scene() != this)
        return;
    initializeDialogWidget();

    if (!player)
        return;
    const QPointF previous = player->pos();
    if (!bagWidget->isVisible() && (!dialogWidget || !dialogWidget->isVisible()))
        QGraphicsScene::advance();

    QGraphicsView* view = views().isEmpty() ? nullptr : views().first();
    if (!view)
        return;

    QPointF center = player->pos() + player->boundingRect().center();
    QRectF bounds = sceneRect();
    QSizeF viewSize = view->viewport()->size();

    qreal centerX = center.x() - viewSize.width() / 2;
    qreal centerY = center.y() - viewSize.height() / 2;

    centerX = qBound(bounds.left(), centerX, bounds.right() - viewSize.width());
    centerY = qBound(bounds.top(), centerY, bounds.bottom() - viewSize.height());

    view->centerOn(QPointF(centerX + viewSize.width() / 2, centerY + viewSize.height() / 2));

    for (auto* item : items(center)) {
        if (item->data(0).toString() == "portal") {
            QString tag = item->data(1).toString();
            if (tag == "toLab") {
                sceneManager->switchTo("lab", QPointF(525, 690));
                return;
            } else if (tag == "toGrassland") {
                sceneManager->switchTo("Grassland", QPointF(515, 1580));
                return;
            } else if (tag == "toTownFromGrass") {
                sceneManager->switchTo("Town", QPointF(540, 6));
                return;
            } else if (tag == "toTownFromLab") {
                sceneManager->switchTo("Town", QPointF(685, 680));
                return;
            }
        }
    }

    if (mapName == "lab" && starterChosen && !starterBalls.isEmpty()) {
        for (auto* ball : starterBalls) {
            removeItem(ball);
            delete ball;
        }
        starterBalls.clear();
    }

    bool shouldShowAKey = false;
    qreal radius = 32.0;
    QRectF detectRange(center.x() - radius, center.y() - radius, radius * 2, radius * 2);

    for (auto* item : items(detectRange)) {
        QString type = item->data(0).toString();
        if (type == "box" || type == "starter_ball" || type == "sign") {
            shouldShowAKey = true;
            break;
        }
    }
    updateAKeyIcon(shouldShowAKey);
    if (player->pos() != previous)
        checkTallGrassEncounter();
}

void ExplorationScene::keyPressEvent(QKeyEvent* ev) {
    initializeDialogWidget();
    if (!dialogWidget || !bagWidget)
        return;
    PlayerItem::Direction direction = PlayerItem::NoneDir;
    switch (ev->key()) {
    case Qt::Key_Up:
        direction = PlayerItem::Up;
        break;
    case Qt::Key_Down:
        direction = PlayerItem::Down;
        break;
    case Qt::Key_Left:
        direction = PlayerItem::Left;
        break;
    case Qt::Key_Right:
        direction = PlayerItem::Right;
        break;
    default:
        break;
    }
    if (direction != PlayerItem::NoneDir) {
        if (!bagWidget->isVisible() && !dialogWidget->isVisible())
            player->setDirection(direction);
        return;
    }
    switch (ev->key()) {
    case Qt::Key_A: {
        // 1. 如果對話框已開啟 → 顯示下一頁或關閉
        if (dialogWidget->isVisible()) {
            if (isNpcDialog && dialogPage + 1 < npcDialogPages.size()) {
                dialogPage++;
                showDialogAtPlayerCenter(npcDialogPages[dialogPage]);
                isNpcDialog = true;
            } else {
                dialogWidget->hide();
                dialogPage = 0;
            }
            return;
        }

        // 2. 取得玩家中心與範圍
        QPointF center = player->pos() + player->boundingRect().center();
        qreal radius = 32.0;
        QRectF detectRange(center.x() - radius, center.y() - radius, radius * 2, radius * 2);

        // 3. 處理箱子
        for (auto* it : items(detectRange)) {
            if (it->data(0).toString() == "box") {
                int index = QRandomGenerator::global()->bounded(3);
                QString item = (index == 0) ? "Pokeball" : (index == 1) ? "Potion" : "Ether";

                bool success = sceneManager->addItem(item);
                if (success) {
                    bagWidget->updateItems(sceneManager->getBagItems(),
                                           sceneManager->getPokemonList());
                    showDialogAtPlayerCenter(QString("You got %1").arg(item));
                    removeItem(it);
                    delete it;
                }
                return;
            }
        }

        // 4. 處理初始寶可夢球
        for (auto* item : items(detectRange)) {
            if (item->data(0).toString() == "starter_ball" && !starterChosen) {
                QString name = item->data(1).toString();
                PokemonItem* starter = new PokemonItem(name);
                sceneManager->addPokemon(starter);
                bagWidget->updateItems(sceneManager->getBagItems(), sceneManager->getPokemonList());

                starterChosen = true;
                dialogPage = 0;

                // 改用獨立的對話框顯示
                showDialogAtPlayerCenter(QString("You chose %1").arg(name));

                for (auto* other : items()) {
                    if (other->data(0).toString() == "starter_ball") {
                        other->setData(0, "inactive_ball");
                    }
                }
                return;
            }
        }

        // 5. 處理 NPC（以 Y 座標判斷為簡單例子）
        if (mapName == "lab" && npc) {
            QRectF npcRect = npc->boundingRect().translated(npc->pos());
            if (npcRect.intersects(detectRange)) {
                dialogPage = 0;
                showDialogAtPlayerCenter(npcDialogPages[dialogPage]);
                isNpcDialog = true;
                return;
            }
        }

        // 6. 處理招牌
        for (auto it = signTexts.constBegin(); it != signTexts.constEnd(); ++it) {
            if (detectRange.intersects(it->first)) {
                showDialogAtPlayerCenter(it->second);
                return;
            }
        }

        break;
    }

    case Qt::Key_B:
        player->setDirection(PlayerItem::NoneDir);
        if (bagWidget->isVisible()) {
            bagWidget->hide();
            bagVisible = false;
        } else {
            QGraphicsView* view = views().isEmpty() ? nullptr : views().first();
            if (view) {
                QSizeF viewSize = view->viewport()->size();
                QPoint centerPos =
                    view->mapToGlobal(QPoint(viewSize.width() / 2 - bagWidget->width() / 2,
                                             viewSize.height() / 2 - bagWidget->height() / 2));
                bagWidget->move(centerPos);
            }
            bagWidget->updateItems(sceneManager->getBagItems(), sceneManager->getPokemonList());
            bagWidget->show();
            bagVisible = true;
        }
        break;

    case Qt::Key_M:
        sceneManager->switchTo("Menu");
        break;

    default:
        QGraphicsScene::keyPressEvent(ev);
    }
}

void ExplorationScene::keyReleaseEvent(QKeyEvent* ev) {
    switch (ev->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
        player->setDirection(PlayerItem::NoneDir);
        break;
    default:
        QGraphicsScene::keyReleaseEvent(ev);
    }
}

void ExplorationScene::setPlayerPosition(const QPointF& pos) {
    if (player)
        player->setPos(pos);
}

void ExplorationScene::setSignText(const QString& text) {
    if (dialogLabel) {
        dialogLabel->setText(text);
    }
}

void ExplorationScene::showDialogAtPlayerCenter(const QString& text) {
    initializeDialogWidget();
    player->setDirection(PlayerItem::NoneDir);
    isNpcDialog = false;
    if (!dialogWidget || !dialogLabel)
        return;

    dialogLabel->setText(text);
    dialogWidget->adjustSize();

    QGraphicsView* view = views().isEmpty() ? nullptr : views().first();
    if (!view)
        return;

    // 設定為視窗內部中心靠下
    int dialogX = (view->width() - dialogWidget->width()) / 2;
    int dialogY = view->height() - dialogWidget->height() - 20;

    dialogWidget->move(dialogX, dialogY);
    dialogWidget->show();
}

void ExplorationScene::initializeDialogWidget() {
    if (!dialogWidget && !views().isEmpty()) {
        QGraphicsView* view = views().first();

        dialogWidget = new QWidget(view->viewport());
        dialogWidget->setFixedSize(525, 100);
        dialogWidget->setWindowFlags(Qt::FramelessWindowHint);

        // 設定背景圖片（保證載入成功）
        QPalette pal;
        QPixmap bg(":/new/prefix1/dialog.png");
        if (!bg.isNull()) {
            pal.setBrush(dialogWidget->backgroundRole(), QBrush(bg));
            dialogWidget->setAutoFillBackground(true);
            dialogWidget->setPalette(pal);
        } else {
            qWarning() << "載入對話框背景失敗";
        }

        QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
        layout->setContentsMargins(20, 10, 20, 10);

        dialogLabel = new QLabel();
        dialogLabel->setStyleSheet("color: black; font-size: 16px;");
        dialogLabel->setWordWrap(true);

        layout->addWidget(dialogLabel);
        dialogWidget->setLayout(layout);

        dialogWidget->move((view->viewport()->width() - 525) / 2, view->viewport()->height() - 120);
        dialogWidget->hide();
    }
}

void ExplorationScene::updateAKeyIcon(bool visible) {
    QGraphicsView* view = views().isEmpty() ? nullptr : views().first();
    if (!view)
        return;

    if (!aKeyIconLabel) {
        aKeyIconLabel = new QLabel(view);
        aKeyIconLabel->setPixmap(QPixmap(":/new/prefix1/A Key.png").scaled(48, 48));
        aKeyIconLabel->setStyleSheet("background: transparent;");
        aKeyIconLabel->resize(48, 48);
        aKeyIconLabel->move(20, view->height() - 68); // 左下角
    }

    aKeyIconLabel->setVisible(visible);
}
void ExplorationScene::checkTallGrassEncounter() {
    if (mapName != "GrassLand" || views().isEmpty() || views().first()->scene() != this)
        return;
    if (!isInTallGrassArea())
        return;

    double chance = QRandomGenerator::global()->generateDouble();

    const QVector<PokemonItem*>& playerPokemons = sceneManager->getPokemonList();
    bool healthy = false;
    for (auto* pokemon : playerPokemons)
        if (pokemon && pokemon->getRemainedHP() > 0)
            healthy = true;
    if (chance < 0.2 && healthy) {
        QStringList wildCandidates = {"Squirtle", "Charmander", "Bulbasaur"};
        int r = QRandomGenerator::global()->bounded(wildCandidates.size());
        PokemonItem* wild = new PokemonItem(wildCandidates[r]);

        player->setDirection(PlayerItem::NoneDir);
        sceneManager->setBattleContext(wild);
        sceneManager->switchTo("Battle", player->pos());
    }
}

ExplorationScene::~ExplorationScene() {
    delete bagWidget;
    delete dialogWidget;
    delete aKeyIconLabel;
}
void ExplorationScene::onSceneEntered() {
    player->setEnabled(true);
    player->setDirection(PlayerItem::NoneDir);
    initializeDialogWidget();
    advanceTimer->start();
    if (npcTimer)
        npcTimer->start();
    sceneManager->setItemOverflowCallback(
        [this](const QString& msg) { showDialogAtPlayerCenter(msg); });
}
void ExplorationScene::onSceneLeft() {
    advanceTimer->stop();
    if (npcTimer)
        npcTimer->stop();
    player->setDirection(PlayerItem::NoneDir);
    player->setEnabled(false);
    if (bagWidget)
        bagWidget->hide();
    if (dialogWidget)
        dialogWidget->hide();
    if (aKeyIconLabel)
        aKeyIconLabel->hide();
    sceneManager->setItemOverflowCallback({});
}
