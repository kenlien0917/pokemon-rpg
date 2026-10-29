#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include <QVector>
#include <QPointF>
#include "PokemonItem.h"
#include <functional>
#include <memory>

class SceneManager : public QObject {
    Q_OBJECT
  public:
    SceneManager(QGraphicsView* view, QObject* parent = nullptr);
    ~SceneManager() override;
    void resetGame();

    void registerScene(const QString& name, QGraphicsScene* scene);
    void switchTo(const QString& name);
    void switchTo(const QString& name, const QPointF& spawnPos);
    QString getCurrentSceneName() const;

    QMap<QString, int>& getBagItems();
    QVector<PokemonItem*>& getPokemonList(); // pointer version
    bool addItem(const QString& itemName);
    void addPokemon(PokemonItem* pokemon);    // pointer version
    void setBattleContext(PokemonItem* wild); // pointer version
    void levelUpPokemon(PokemonItem* target);
    void setItemOverflowCallback(std::function<void(const QString&)> callback);
    void setItemCount(const QString& itemName, int count);

  private:
    QGraphicsView* view;
    QMap<QString, QGraphicsScene*> scenes;
    QMap<QString, int> bagItems;
    QVector<PokemonItem*> pokemonList;
    std::unique_ptr<PokemonItem> battleWild;
    std::function<void(const QString&)> itemOverflowCallback;
    void switchScene(const QString& name, const QPointF* spawnPos);
};

#endif // SCENEMANAGER_H
