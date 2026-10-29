#include <QtTest>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include "PokemonItem.h"
#include "scenemanager.h"
#include "BattleScene.h"
#include "ExplorationScene.h"
#include "PlayerItem.h"
#include "BagWidget.h"
#include "MenuScene.h"

class GameTests : public QObject {
    Q_OBJECT
  private slots:
    void partySelectorCanScrollToCapturedPokemon() {
        QGraphicsView view;
        SceneManager manager(&view);
        for (int i = 0; i < 7; ++i)
            manager.addPokemon(new PokemonItem("Squirtle"));
        BattleScene battle(&manager);
        battle.initBattle(new PokemonItem("Bulbasaur"));
        QMetaObject::invokeMethod(&battle, "onPokemon", Qt::DirectConnection);
        QScrollArea* scroll = nullptr;
        for (auto* item : battle.items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            if (proxy)
                if (auto* area = qobject_cast<QScrollArea*>(proxy->widget()))
                    scroll = area;
        }
        QVERIFY2(scroll, "Party selection must remain usable beyond four Pokemon");
        QCOMPARE(scroll->widget()->findChildren<QPushButton*>().size(), 7);
    }
    void grassSkillsStayInTheirEvolutionFamily() {
        auto grass = assignSkillsForPokemon("Ivysaur", 4);
        auto fire = assignSkillsForPokemon("Charmeleon", 4);
        QCOMPARE(grass.size(), 4);
        QCOMPARE(grass[2].name, QString("Growth"));
        QCOMPARE(grass[3].name, QString("Razor Leaf"));
        QCOMPARE(fire.size(), 4);
        QCOMPARE(fire[3].name, QString("Flare Blitz"));
    }
    void healthIsBounded() {
        PokemonItem pokemon("Squirtle");
        pokemon.setRemainedHP(-10);
        QCOMPARE(pokemon.getRemainedHP(), 0);
        pokemon.setRemainedHP(1000);
        QCOMPARE(pokemon.getRemainedHP(), pokemon.getMaxHP());
    }
    void levelUpPreservesReferences() {
        QGraphicsView view;
        SceneManager manager(&view);
        auto* pokemon = new PokemonItem("Squirtle", 2);
        manager.addPokemon(pokemon);
        manager.levelUpPokemon(pokemon);
        QVERIFY(manager.getPokemonList().first() == pokemon);
        QCOMPARE(pokemon->getName(), QString("Wartortle"));
        QCOMPARE(pokemon->getLevel(), 3);
    }
    void bagReusesItsItemLabels() {
        BagWidget bag;
        QMap<QString, int> items{{"Potion", 1}};
        QVector<PokemonItem*> party;
        bag.updateItems(items, party);
        QPointer<QLabel> icon = bag.findChild<QLabel*>("item_icon_Potion");
        QVERIFY(icon);
        items["Potion"] = 2;
        bag.updateItems(items, party);
        QVERIFY(icon);
        QCOMPARE(bag.findChild<QLabel*>("item_icon_Potion"), icon.data());
        QCOMPARE(bag.findChild<QLabel*>("item_count_Potion")->text(), QString("x2"));
    }
    void movementChecksTheDestinationAndDisabledState() {
        QGraphicsScene scene;
        scene.setSceneRect(0, 0, 100, 100);
        auto* player = new PlayerItem;
        QPixmap image(10, 10);
        image.fill(Qt::red);
        player->setPixmap(image);
        scene.addItem(player);
        auto* wall = scene.addRect(QRectF(6, 0, 2, 10), QPen(Qt::NoPen));
        wall->setData(0, "barrier");
        player->setDirection(PlayerItem::Right);
        scene.advance();
        QCOMPARE(player->pos(), QPointF(0, 0));
        scene.removeItem(wall);
        delete wall;
        player->setEnabled(false);
        scene.advance();
        QCOMPARE(player->pos(), QPointF(0, 0));
    }
    void repeatedAttacksSpendOnlyOneTurn() {
        QGraphicsView view;
        SceneManager manager(&view);
        manager.addPokemon(new PokemonItem("Squirtle"));
        BattleScene battle(&manager);
        battle.initBattle(new PokemonItem("Bulbasaur"));
        QVERIFY(QMetaObject::invokeMethod(&battle, "onFight", Qt::DirectConnection));
        QPushButton* attack = nullptr;
        for (auto* item : battle.items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            auto* button = proxy ? qobject_cast<QPushButton*>(proxy->widget()) : nullptr;
            if (button && button->text().startsWith("Tackle"))
                attack = button;
        }
        QVERIFY(attack);
        attack->click();
        attack->click();
        QCOMPARE(manager.getPokemonList().first()->getSkillsRef().first().remainedPP, 19);
    }
    void overflowCallbacksBelongToEachGame() {
        QGraphicsView firstView, secondView;
        SceneManager first(&firstView), second(&secondView);
        int firstCalls = 0, secondCalls = 0;
        first.setItemOverflowCallback([&](const QString&) { ++firstCalls; });
        second.setItemOverflowCallback([&](const QString&) { ++secondCalls; });
        first.setItemCount("Pokeball", 3);
        QVERIFY(!first.addItem("Pokeball"));
        QCOMPARE(firstCalls, 1);
        QCOMPARE(secondCalls, 0);
    }
    void battleEntrySelectsAHealthyPokemon() {
        QGraphicsView view;
        SceneManager manager(&view);
        auto* fainted = new PokemonItem("Squirtle");
        fainted->setRemainedHP(0);
        auto* healthy = new PokemonItem("Charmander");
        manager.addPokemon(fainted);
        manager.addPokemon(healthy);
        BattleScene battle(&manager);
        battle.initBattle(new PokemonItem("Bulbasaur"));
        QMetaObject::invokeMethod(&battle, "onFight", Qt::DirectConnection);
        for (auto* item : battle.items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            auto* button = proxy ? qobject_cast<QPushButton*>(proxy->widget()) : nullptr;
            if (button && button->text().startsWith("Scratch")) {
                button->click();
                break;
            }
        }
        QCOMPARE(healthy->getSkillsRef().first().remainedPP, 19);
        QCOMPARE(fainted->getSkillsRef().first().remainedPP, 20);
    }
    void leavingBattleCancelsThePendingEnemyTurn() {
        QGraphicsView view;
        SceneManager manager(&view);
        auto* pokemon = new PokemonItem("Squirtle");
        manager.addPokemon(pokemon);
        auto* battle = new BattleScene(&manager);
        auto* town = new QGraphicsScene(&manager);
        manager.registerScene("Battle", battle);
        manager.registerScene("Town", town);
        battle->initBattle(new PokemonItem("Bulbasaur"));
        manager.switchTo("Battle");
        QMetaObject::invokeMethod(battle, "onFight", Qt::DirectConnection);
        for (auto* item : battle->items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            auto* button = proxy ? qobject_cast<QPushButton*>(proxy->widget()) : nullptr;
            if (button && button->text().startsWith("Tackle")) {
                button->click();
                break;
            }
        }
        manager.switchTo("Town");
        QTest::qWait(1650);
        QCOMPARE(pokemon->getRemainedHP(), pokemon->getMaxHP());
    }
    void startingAgainClearsThePreviousGame() {
        QGraphicsView view;
        SceneManager manager(&view);
        manager.addPokemon(new PokemonItem("Squirtle"));
        manager.setItemCount("Potion", 2);
        manager.registerScene("Town", new ExplorationScene(&manager, "Town"));
        MenuScene menu(&manager);
        menu.mousePressEvent(nullptr);
        QVERIFY(manager.getPokemonList().isEmpty());
        QVERIFY(manager.getBagItems().isEmpty());
        QCOMPARE(manager.getCurrentSceneName(), QString("Town"));
    }
    void realMapsKeepTheirPortalsAndStarterReachable() {
        QGraphicsView view;
        SceneManager manager(&view);
        auto* town = new ExplorationScene(&manager, "Town");
        auto* lab = new ExplorationScene(&manager, "lab");
        manager.registerScene("Town", town);
        manager.registerScene("lab", lab);
        manager.registerScene("Grassland", new QGraphicsScene(&manager));
        manager.switchTo("Town", QPointF(524, 0));
        QKeyEvent up(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
        QApplication::sendEvent(town, &up);
        for (int i = 0; i < 30 && manager.getCurrentSceneName() == "Town"; ++i)
            QMetaObject::invokeMethod(town, "advance", Qt::DirectConnection);
        QCOMPARE(manager.getCurrentSceneName(), QString("Grassland"));
        manager.switchTo("lab", QPointF(525, 690));
        QKeyEvent down(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        QApplication::sendEvent(lab, &down);
        for (int i = 0; i < 20 && manager.getCurrentSceneName() == "lab"; ++i)
            QMetaObject::invokeMethod(lab, "advance", Qt::DirectConnection);
        QCOMPARE(manager.getCurrentSceneName(), QString("Town"));
        manager.switchTo("lab", QPointF(565, 486));
        QApplication::sendEvent(lab, &up);
        for (int i = 0; i < 20; ++i)
            QMetaObject::invokeMethod(lab, "advance", Qt::DirectConnection);
        QKeyEvent interact(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
        QApplication::sendEvent(lab, &interact);
        QCOMPARE(manager.getPokemonList().size(), 1);
    }
    void reenteringBattleDoesNotKeepDeletedBagButtons() {
        QGraphicsView view;
        SceneManager manager(&view);
        manager.addPokemon(new PokemonItem("Squirtle"));
        manager.setItemCount("Potion", 2);
        BattleScene battle(&manager);
        for (int i = 0; i < 3; ++i) {
            battle.initBattle(new PokemonItem("Bulbasaur"));
            QVERIFY(QMetaObject::invokeMethod(&battle, "onBag", Qt::DirectConnection));
        }
        QCOMPARE(manager.getBagItems().value("Potion"), 2);
    }
    void forcedSwitchDoesNotGiveAnExtraEnemyTurn() {
        QGraphicsView view;
        SceneManager manager(&view);
        auto* first = new PokemonItem("Squirtle");
        first->setRemainedHP(1);
        auto* second = new PokemonItem("Wartortle", 3);
        manager.addPokemon(first);
        manager.addPokemon(second);
        BattleScene battle(&manager);
        battle.initBattle(new PokemonItem("Bulbasaur"));
        QMetaObject::invokeMethod(&battle, "onFight", Qt::DirectConnection);
        for (auto* item : battle.items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            auto* button = proxy ? qobject_cast<QPushButton*>(proxy->widget()) : nullptr;
            if (button && button->text().startsWith("Tackle")) {
                button->click();
                break;
            }
        }
        QTest::qWait(2650);
        QCOMPARE(first->getRemainedHP(), 0);
        QPushButton* choose = nullptr;
        for (auto* item : battle.items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            if (proxy)
                choose = proxy->widget()->findChild<QPushButton*>();
            if (choose && choose->text() == "Wartortle")
                break;
            choose = nullptr;
        }
        QVERIFY(choose);
        choose->click();
        bool correctHpRange = false;
        for (auto* item : battle.items()) {
            auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
            auto* bar = proxy ? qobject_cast<QProgressBar*>(proxy->widget()) : nullptr;
            if (bar && bar->maximum() == second->getMaxHP())
                correctHpRange = true;
        }
        QVERIFY(correctHpRange);
        QTest::qWait(1600);
        QCOMPARE(second->getRemainedHP(), second->getMaxHP());
    }
};
QTEST_MAIN(GameTests)
#include "game_tests.moc"
