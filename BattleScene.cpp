#include "BattleScene.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QVBoxLayout>

BattleScene::BattleScene(SceneManager* mgr, QObject* parent)
    : QGraphicsScene(parent), sceneManager(mgr) {}

void BattleScene::setPhase(Phase next) {
    phase = next;
    for (int i = 0; i < actionProxies.size(); ++i)
        actionProxies[i]->setEnabled(next == Phase::PlayerTurn ||
                                     (next == Phase::ChoosingPokemon && i == 2));
}
void BattleScene::schedule(int delay, std::function<void()> action) {
    const auto token = generation;
    QTimer::singleShot(delay, this, [this, token, action]() {
        if (token == generation && phase != Phase::Inactive)
            action();
    });
}
void BattleScene::clearMenus() {
    for (auto* proxy : menuProxies) {
        proxy->hide();
        removeItem(proxy);
        proxy->deleteLater();
    }
    menuProxies.clear();
}
void BattleScene::leaveBattle() {
    ++generation;
    setPhase(Phase::Inactive);
    clearMenus();
    wildPokemon.reset();
    playerPokemon = nullptr;
    currentPokemonIndex = -1;
}

void BattleScene::setupUI() {
    const QStringList names{"Fight", "Bag", "Pokemon", "Run"};
    void (BattleScene::* const handlers[])() = {&BattleScene::onFight, &BattleScene::onBag,
                                                &BattleScene::onPokemon, &BattleScene::onRun};
    for (int i = 0; i < names.size(); ++i) {
        QPixmap pixmap(QString(":/new/prefix1/battle/%1.png").arg(names[i]));
        auto* button = new QPushButton;
        button->setIcon(QIcon(pixmap));
        button->setIconSize(pixmap.size());
        button->setFlat(true);
        button->setStyleSheet("background: transparent; border: none;");
        auto* proxy = addWidget(button);
        proxy->setPos(300 + (i % 2) * (pixmap.width() + 70),
                      350 + (i / 2) * (pixmap.height() + 10));
        actionProxies.append(proxy);
        connect(button, &QPushButton::clicked, this, handlers[i]);
    }
}

void BattleScene::initBattle(PokemonItem* wild) {
    leaveBattle();
    clear();
    actionProxies.clear();
    messageLabel = nullptr;
    playerImage = nullptr;
    wildImage = nullptr;
    playerHpBar = nullptr;
    wildHpBar = nullptr;
    playerNameText = nullptr;
    playerHpText = nullptr;
    playerLevelText = nullptr;
    wildNameText = nullptr;
    wildHpText = nullptr;
    wildLevelText = nullptr;
    wildPokemon.reset(wild);
    pokemonList = &sceneManager->getPokemonList();
    setSceneRect(0, 0, 525, 450);
    if (!wildPokemon)
        return;
    for (int i = 0; i < pokemonList->size(); ++i)
        if ((*pokemonList)[i] && (*pokemonList)[i]->getRemainedHP() > 0) {
            currentPokemonIndex = i;
            break;
        }
    if (currentPokemonIndex < 0) {
        showGameOver();
        return;
    }
    playerPokemon = (*pokemonList)[currentPokemonIndex];
    const int playerHp = playerPokemon->getRemainedHP();
    const int wildHp = wildPokemon->getRemainedHP();
    bgItem = addPixmap(QPixmap(":/new/prefix1/battle/battle_scene.png"));
    bgItem->setZValue(-1);
    QFont nameFont("Impact", 12);
    QFont levelFont("Impact", 12);
    QColor textColor = Qt::black;

    playerNameText = addText(playerPokemon->getName(), nameFont);
    playerNameText->setDefaultTextColor(textColor);
    playerNameText->setPos(310, 220);

    wildNameText = addText(wildPokemon->getName(), nameFont);
    wildNameText->setDefaultTextColor(textColor);
    wildNameText->setPos(40, 55);

    playerLevelText = addText(QString("Lv. %1").arg(playerPokemon->getLevel()), levelFont);
    playerLevelText->setDefaultTextColor(textColor);
    playerLevelText->setPos(450, 223);

    wildLevelText = addText(QString("Lv. %1").arg(wildPokemon->getLevel()), levelFont);
    wildLevelText->setDefaultTextColor(textColor);
    wildLevelText->setPos(180, 58);

    playerHpText =
        addText(QString("%1 / %2").arg(playerHp).arg(playerPokemon->getMaxHP()), nameFont);
    playerHpText->setDefaultTextColor(textColor);
    playerHpText->setPos(425, 270);

    wildHpText = addText(QString("%1 / %2").arg(wildHp).arg(wildPokemon->getMaxHP()), nameFont);
    wildHpText->setDefaultTextColor(textColor);
    wildHpText->setPos(155, 115);

    playerImage = addPixmap(playerPokemon->getBackImage());
    playerImage->setPos(70, 210);
    playerImage->setZValue(1);

    wildImage = addPixmap(wildPokemon->getFrontImage());
    wildImage->setPos(320, 80);
    wildImage->setZValue(1);

    playerHpBar = new QProgressBar();
    playerHpBar->setRange(0, playerPokemon->getMaxHP());
    playerHpBar->setValue(playerHp);
    playerHpBar->setTextVisible(false);
    playerHpBar->setFixedSize(110, 12);
    playerHpBar->setStyleSheet("QProgressBar { border: 1px solid black; background-color: #ccc; }"
                               "QProgressBar::chunk { background-color: #4CAF50; }");
    QGraphicsProxyWidget* playerHpBarProxy = addWidget(playerHpBar);
    playerHpBarProxy->setPos(380, 258);

    wildHpBar = new QProgressBar();
    wildHpBar->setRange(0, wildPokemon->getMaxHP());
    wildHpBar->setValue(wildHp);
    wildHpBar->setTextVisible(false);
    wildHpBar->setFixedSize(110, 12);
    wildHpBar->setStyleSheet("QProgressBar { border: 1px solid black; background-color: #ccc; }"
                             "QProgressBar::chunk { background-color: #4CAF50; }");
    QGraphicsProxyWidget* wildHpBarProxy = addWidget(wildHpBar);
    wildHpBarProxy->setPos(111, 93);

    setupUI();
    setPhase(Phase::PlayerTurn);
    showMessage("What will " + playerPokemon->getName() + " do?");
}

void BattleScene::refreshPlayerDisplay() {
    playerImage->setPixmap(playerPokemon->getBackImage());
    playerNameText->setPlainText(playerPokemon->getName());
    playerLevelText->setPlainText(QString("Lv. %1").arg(playerPokemon->getLevel()));
    playerHpText->setPlainText(
        QString("%1 / %2").arg(playerPokemon->getRemainedHP()).arg(playerPokemon->getMaxHP()));
    playerHpBar->setRange(0, playerPokemon->getMaxHP());
    playerHpBar->setValue(playerPokemon->getRemainedHP());
}

void BattleScene::onFight() {
    if (phase != Phase::PlayerTurn || !playerPokemon)
        return;
    clearMenus();
    const auto& skills = playerPokemon->getSkillsRef();
    for (int i = 0; i < skills.size(); ++i) {
        const auto& skill = skills[i];
        auto* button = new QPushButton(
            QString("%1 (%2/%3)").arg(skill.name).arg(skill.remainedPP).arg(skill.pp));
        button->setFixedSize(200, 40);
        button->setStyleSheet("font: 12pt 'Impact'; color: black;");
        auto* proxy = addWidget(button);
        proxy->setPos(30 + (i % 2) * 230, 340 + (i / 2) * 50);
        proxy->setZValue(10);
        menuProxies.append(proxy);
        connect(button, &QPushButton::clicked, this, [this, i]() {
            if (phase != Phase::PlayerTurn)
                return;
            const Skill skill = playerPokemon->getSkillsRef()[i];
            if (skill.remainedPP <= 0) {
                showMessage(skill.name + " has no PP left!");
                return;
            }
            setPhase(Phase::Waiting);
            clearMenus();
            const int damage =
                qMax(1, (skill.power + playerPokemon->getAttack() - wildPokemon->getDefense()) *
                            playerPokemon->getLevel());
            wildPokemon->setRemainedHP(wildPokemon->getRemainedHP() - damage);
            playerPokemon->useSkillPP(i);
            wildHpBar->setValue(wildPokemon->getRemainedHP());
            wildHpText->setPlainText(
                QString("%1 / %2").arg(wildPokemon->getRemainedHP()).arg(wildPokemon->getMaxHP()));
            showMessage(QString("%1 used %2!\n%3 takes %4 damage!")
                            .arg(playerPokemon->getName())
                            .arg(skill.name)
                            .arg(wildPokemon->getName())
                            .arg(damage));
            if (wildPokemon->getRemainedHP() == 0) {
                showMessage(playerPokemon->getName() + " won!");
                sceneManager->levelUpPokemon(playerPokemon);
                setPhase(Phase::Finished);
                schedule(1500, [this]() { sceneManager->switchTo("Grassland"); });
            } else
                schedule(1500, [this]() { enemyTurn(); });
        });
    }
}
void BattleScene::onRun() {
    if (phase == Phase::PlayerTurn)
        sceneManager->switchTo("Grassland");
}
void BattleScene::onBag() {
    if (phase != Phase::PlayerTurn)
        return;
    clearMenus();
    const QStringList names{"Pokeball", "Ether", "Potion"};
    int row = 0;
    for (const auto& name : names) {
        if (sceneManager->getBagItems().value(name) <= 0)
            continue;
        auto* button = new QPushButton(name);
        button->setIcon(QIcon(QString(":/new/prefix1/icon/%1_bag.png").arg(name)));
        button->setIconSize(QSize(32, 32));
        button->setStyleSheet("font: 12pt 'Impact'; color: black;");
        auto* proxy = addWidget(button);
        proxy->setPos(30, 300 + row++ * 40);
        proxy->setZValue(10);
        menuProxies.append(proxy);
        connect(button, &QPushButton::clicked, this, [this, name]() {
            if (phase != Phase::PlayerTurn || sceneManager->getBagItems().value(name) <= 0)
                return;
            setPhase(Phase::Waiting);
            clearMenus();
            sceneManager->setItemCount(name, sceneManager->getBagItems().value(name) - 1);
            if (name == "Ether") {
                for (auto& skill : playerPokemon->getSkillsRef())
                    skill.remainedPP = skill.pp;
                showMessage("PP fully restored!");
            } else if (name == "Potion") {
                playerPokemon->setRemainedHP(playerPokemon->getRemainedHP() + 10);
                refreshPlayerDisplay();
                showMessage("HP restored by 10!");
            } else {
                const double chance =
                    double(wildPokemon->getMaxHP() - wildPokemon->getRemainedHP()) /
                    wildPokemon->getMaxHP();
                if (QRandomGenerator::global()->generateDouble() < chance) {
                    showMessage("You caught " + wildPokemon->getName() + "!");
                    setPhase(Phase::Finished);
                    schedule(1500, [this]() {
                        sceneManager->addPokemon(wildPokemon.release());
                        sceneManager->switchTo("Grassland");
                    });
                    return;
                }
                showMessage("You failed to catch " + wildPokemon->getName() + ".");
            }
            schedule(1500, [this]() { enemyTurn(); });
        });
    }
}
void BattleScene::onPokemon() {
    if (phase != Phase::PlayerTurn && phase != Phase::ChoosingPokemon)
        return;
    clearMenus();
    auto* selector = new QScrollArea;
    selector->setFixedSize(300, 300);
    selector->setWidgetResizable(true);
    auto* content = new QWidget;
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(5, 5, 5, 5);
    selector->setWidget(content);
    selector->setStyleSheet("QWidget { border-image: url(:/new/prefix1/battle/PokemonList.png); }");
    auto* proxy = addWidget(selector);
    proxy->setPos(100, 70);
    proxy->setZValue(15);
    menuProxies.append(proxy);
    for (int i = 0; i < pokemonList->size(); ++i) {
        auto* pokemon = (*pokemonList)[i];
        if (!pokemon || pokemon->getRemainedHP() <= 0)
            continue;
        auto* button = new QPushButton(pokemon->getName(), content);
        button->setMinimumHeight(70);
        layout->addWidget(button);
        button->setIcon(QIcon(pokemon->getFrontImage()));
        button->setIconSize(QSize(48, 48));
        button->setEnabled(i != currentPokemonIndex);
        connect(button, &QPushButton::clicked, this, [this, i]() {
            if (phase != Phase::PlayerTurn && phase != Phase::ChoosingPokemon)
                return;
            const bool forced = phase == Phase::ChoosingPokemon;
            currentPokemonIndex = i;
            playerPokemon = (*pokemonList)[i];
            clearMenus();
            refreshPlayerDisplay();
            showMessage("Go! " + playerPokemon->getName() + "!");
            setPhase(forced ? Phase::PlayerTurn : Phase::Waiting);
            if (!forced)
                schedule(1500, [this]() { enemyTurn(); });
        });
    }
    layout->addStretch();
}

void BattleScene::showMessage(const QString& text) {
    if (!messageLabel) {
        messageLabel = new QLabel;
        messageLabel->setStyleSheet(
            "QLabel { color: white; font: 12pt 'Impact'; background-color: transparent; }");
        messageLabel->setWordWrap(true);
        auto* proxy = addWidget(messageLabel);
        proxy->setPos(25, 335);
        proxy->resize(200, 95);
        proxy->setZValue(9);
    }
    messageLabel->setText(text);
    messageLabel->show();
}
void BattleScene::enemyTurn() {
    if (phase != Phase::Waiting || !wildPokemon || !playerPokemon)
        return;
    const auto& skills = wildPokemon->getSkillsRef();
    if (skills.isEmpty()) {
        setPhase(Phase::PlayerTurn);
        return;
    }
    const Skill skill = skills[QRandomGenerator::global()->bounded(skills.size())];
    const int damage =
        qMax(1, (skill.power + wildPokemon->getAttack() - playerPokemon->getDefense()) *
                    wildPokemon->getLevel());
    playerPokemon->setRemainedHP(playerPokemon->getRemainedHP() - damage);
    refreshPlayerDisplay();
    showMessage(QString("%1 used %2!\n%3 takes %4 damage!")
                    .arg(wildPokemon->getName())
                    .arg(skill.name)
                    .arg(playerPokemon->getName())
                    .arg(damage));
    schedule(1000, [this]() {
        if (allPlayerFainted()) {
            showGameOver();
            return;
        }
        if (playerPokemon->getRemainedHP() == 0) {
            setPhase(Phase::ChoosingPokemon);
            showMessage("Choose another Pokémon!");
            onPokemon();
        } else {
            setPhase(Phase::PlayerTurn);
            showMessage("What will " + playerPokemon->getName() + " do?");
        }
    });
}
bool BattleScene::allPlayerFainted() const {
    if (!pokemonList)
        return true;
    for (auto* pokemon : *pokemonList)
        if (pokemon && pokemon->getRemainedHP() > 0)
            return false;
    return true;
}
void BattleScene::showGameOver() {
    ++generation;
    setPhase(Phase::Finished);
    clearMenus();
    clear();
    actionProxies.clear();
    messageLabel = nullptr;
    QPixmap image(":/new/prefix1/scene/game_over.png");
    addPixmap(image)->setPos((525 - image.width()) / 2, (450 - image.height()) / 2);
    schedule(2000, [this]() { sceneManager->switchTo("Menu"); });
}
