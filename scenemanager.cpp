#include "scenemanager.h"
#include "ExplorationScene.h"
#include "BattleScene.h"
#include <QDebug>

SceneManager::SceneManager(QGraphicsView* view, QObject* parent) : QObject(parent), view(view) {}
SceneManager::~SceneManager() {
    qDeleteAll(pokemonList);
}

void SceneManager::registerScene(const QString& name, QGraphicsScene* scene) {
    if (!scene)
        return;
    scene->setParent(this); // Registered scenes belong to this game.
    scenes[name] = scene;
}
void SceneManager::switchTo(const QString& name) {
    switchScene(name, nullptr);
}
void SceneManager::switchTo(const QString& name, const QPointF& pos) {
    switchScene(name, &pos);
}

void SceneManager::switchScene(const QString& name, const QPointF* spawnPos) {
    if (!view || !scenes.contains(name)) {
        qWarning() << "Scene not found:" << name;
        return;
    }
    auto* old = view->scene();
    auto* next = scenes[name];
    if (old != next) {
        if (auto* exp = qobject_cast<ExplorationScene*>(old))
            exp->onSceneLeft();
        if (auto* battle = qobject_cast<BattleScene*>(old))
            battle->leaveBattle();
    }
    if (auto* battle = qobject_cast<BattleScene*>(next))
        if (battleWild)
            battle->initBattle(battleWild.release());
    view->setScene(next);
    if (auto* exp = qobject_cast<ExplorationScene*>(next)) {
        if (spawnPos)
            exp->setPlayerPosition(*spawnPos);
        exp->onSceneEntered();
    }
}

void SceneManager::resetGame() {
    for (auto* scene : scenes) {
        if (auto* battle = qobject_cast<BattleScene*>(scene))
            battle->leaveBattle();
        if (auto* exp = qobject_cast<ExplorationScene*>(scene))
            exp->onSceneLeft();
    }
    battleWild.reset();
    qDeleteAll(pokemonList);
    pokemonList.clear();
    bagItems.clear();
    itemOverflowCallback = {};
    for (const auto& name : scenes.keys()) {
        auto* exp = qobject_cast<ExplorationScene*>(scenes[name]);
        if (!exp)
            continue;
        const QString map = exp->getMapName();
        if (view && view->scene() == exp)
            view->setScene(nullptr);
        delete exp;
        scenes[name] = new ExplorationScene(this, map, this);
    }
}

QMap<QString, int>& SceneManager::getBagItems() {
    return bagItems;
}
QVector<PokemonItem*>& SceneManager::getPokemonList() {
    return pokemonList;
}
void SceneManager::addPokemon(PokemonItem* pokemon) {
    if (pokemon && !pokemonList.contains(pokemon))
        pokemonList.append(pokemon);
}
void SceneManager::setBattleContext(PokemonItem* wild) {
    battleWild.reset(wild);
}
QString getEvolvedName(const QString& name, int level) {
    if (name == "Squirtle" && level >= 3)
        return "Wartortle";
    if (name == "Wartortle" && level >= 5)
        return "Blastoise";
    if (name == "Charmander" && level >= 3)
        return "Charmeleon";
    if (name == "Charmeleon" && level >= 5)
        return "Charizard";
    if (name == "Bulbasaur" && level >= 3)
        return "Ivysaur";
    if (name == "Ivysaur" && level >= 5)
        return "Venusaur";
    return name; // 沒進化
}

void SceneManager::levelUpPokemon(PokemonItem* target) {
    if (!target)
        return;

    QString oldName = target->getName();
    int newLevel = target->getLevel() + 1;
    QString evolvedName = getEvolvedName(oldName, newLevel);

    // 找出 target 在 pokemonList 中的位置
    for (int i = 0; i < pokemonList.size(); ++i) {
        if (pokemonList[i] == target) {
            *target = PokemonItem(evolvedName, newLevel); // Keep party references valid.
            return;
        }
    }
    qWarning() << "Level-up failed: Pokémon not found in list";
}

QString SceneManager::getCurrentSceneName() const {
    if (!view || !view->scene())
        return "";
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        if (it.value() == view->scene())
            return it.key();
    }
    return "";
}

void SceneManager::setItemOverflowCallback(std::function<void(const QString&)> callback) {
    itemOverflowCallback = callback;
}

bool SceneManager::addItem(const QString& itemName) {
    if (itemName == "Pokeball" && bagItems["Pokeball"] >= 3) {
        if (itemOverflowCallback)
            itemOverflowCallback("You can't carry more than 3 Pokéballs.");
        return false;
    }
    bagItems[itemName]++;
    return true;
}

void SceneManager::setItemCount(const QString& itemName, int count) {
    if (count < 0)
        count = 0;
    bagItems[itemName] = count;
}
