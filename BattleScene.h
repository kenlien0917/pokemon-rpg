#ifndef BATTLESCENE_H
#define BATTLESCENE_H
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QProgressBar>
#include <functional>
#include <memory>
#include "scenemanager.h"

class BattleScene : public QGraphicsScene {
    Q_OBJECT
  public:
    BattleScene(SceneManager* mgr, QObject* parent = nullptr);
    void initBattle(PokemonItem* wild); // Takes ownership until capture transfers it.
    void leaveBattle();
  private slots:
    void onFight();
    void onRun();
    void onBag();
    void onPokemon();

  private:
    enum class Phase { Inactive, PlayerTurn, Waiting, ChoosingPokemon, Finished };
    Phase phase = Phase::Inactive;
    quint64 generation = 0;
    SceneManager* sceneManager;
    PokemonItem* playerPokemon = nullptr; // Borrowed from SceneManager's party.
    std::unique_ptr<PokemonItem> wildPokemon;
    QVector<PokemonItem*>* pokemonList = nullptr;
    int currentPokemonIndex = -1;
    QGraphicsPixmapItem *bgItem = nullptr, *playerImage = nullptr, *wildImage = nullptr;
    QGraphicsTextItem *playerNameText = nullptr, *playerHpText = nullptr, *wildNameText = nullptr,
                      *wildHpText = nullptr;
    QGraphicsTextItem *playerLevelText = nullptr, *wildLevelText = nullptr;
    QProgressBar *playerHpBar = nullptr, *wildHpBar = nullptr;
    QVector<QGraphicsProxyWidget*> actionProxies, menuProxies;
    QLabel* messageLabel = nullptr;
    void setupUI();
    void clearMenus();
    void setPhase(Phase next);
    void schedule(int delay, std::function<void()> action);
    void refreshPlayerDisplay();
    void showGameOver();
    void showMessage(const QString& text);
    void enemyTurn();
    bool allPlayerFainted() const;
};
#endif
