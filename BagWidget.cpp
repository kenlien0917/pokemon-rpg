#include "BagWidget.h"
#include <QEvent>
#include "PokemonItem.h"
#include <QLabel>
#include <QGridLayout>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QSet>

BagWidget::BagWidget(QWidget* parent) : QWidget(parent) {
    setFixedSize(300, 383);

    QLabel* background = new QLabel(this);
    background->setPixmap(QPixmap(":/new/prefix1/bag.png"));
    background->setScaledContents(true);
    background->setFixedSize(size());
    background->move(0, 0); // 確保在左上角
    background->lower();    // 將背景推到底層
}

void BagWidget::updateItems(QMap<QString, int>& itemsRef, QVector<PokemonItem*>& pokemonsRef) {
    QSet<QString> usedLabels;

    // 更新道具圖示與數量
    QStringList keys = {"Pokeball", "Ether", "Potion"};
    QStringList icons = {":/new/prefix1/icon/Pokeball_bag.png", ":/new/prefix1/icon/Ether_bag.png",
                         ":/new/prefix1/icon/Potion_bag.png"};

    int startX = 20;
    int startY = 20;
    int spacingX = 85;

    for (int i = 0; i < keys.size(); ++i) {
        QString key = keys[i];
        QString iconPath = icons[i];
        int count = itemsRef.value(key, 0);
        usedLabels.insert("item_icon_" + key);
        usedLabels.insert("item_count_" + key);

        QLabel* iconLabel = findChild<QLabel*>("item_icon_" + key);
        if (!iconLabel) {
            iconLabel = new QLabel(this);
            iconLabel->setObjectName("item_icon_" + key);
            iconLabel->setPixmap(QPixmap(iconPath).scaled(32, 32));
            iconLabel->move(startX + i * spacingX, startY);
            iconLabel->installEventFilter(this);
            iconLabel->raise();
            iconLabel->show();
        }

        QLabel* countLabel = findChild<QLabel*>("item_count_" + key);
        if (!countLabel) {
            countLabel = new QLabel(this);
            countLabel->setObjectName("item_count_" + key);
            countLabel->move(startX + i * spacingX + 40, startY + 5);
            countLabel->raise();
            countLabel->show();
        }
        countLabel->setText(QString("x%1").arg(count));
        countLabel->resize(50, 20);
        countLabel->setStyleSheet("font-size: 14px; color: black;");
    }

    // 更新寶可夢列表
    int row = 0;
    for (PokemonItem* pkm : pokemonsRef) {
        if (!pkm)
            continue;
        QString base = "pokemon_" + pkm->getName() + "_" + QString::number(row);
        QString imageKey = base + "_img";
        QString nameKey = base + "_name";
        usedLabels.insert(imageKey);
        usedLabels.insert(nameKey);

        QLabel* imgLabel = findChild<QLabel*>(imageKey);
        if (!imgLabel) {
            imgLabel = new QLabel(this);
            imgLabel->setObjectName(imageKey);

            imgLabel->move(20, 80 + row * 60);
            imgLabel->raise();
            imgLabel->show();
        }

        imgLabel->setPixmap(pkm->getFrontImage().scaled(50, 50));
        QLabel* nameLabel = findChild<QLabel*>(nameKey);
        if (!nameLabel) {
            nameLabel = new QLabel(this);
            nameLabel->setObjectName(nameKey);
            nameLabel->move(80, 80 + row * 60 + 10);
            nameLabel->raise();
            nameLabel->show();
        }
        nameLabel->setText(pkm->getName());
        nameLabel->resize(120, 20);
        nameLabel->setStyleSheet("font-size: 14px; color: black;");

        ++row;
    }

    // 移除未使用的標籤
    for (QLabel* label : findChildren<QLabel*>()) {
        if (label->objectName().startsWith("item_") || label->objectName().startsWith("pokemon_")) {
            if (!usedLabels.contains(label->objectName())) {
                delete label;
            }
        }
    }
}

bool BagWidget::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QString id = obj->objectName(); // e.g. "item_icon_Ether"
        if (id.startsWith("item_icon_")) {
            QString itemName = id.section('_', 2);
            emit itemUsed(itemName);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
