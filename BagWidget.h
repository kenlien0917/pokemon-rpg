#ifndef BAGWIDGET_H
#define BAGWIDGET_H

#include "PokemonItem.h"
#include <QWidget>
#include <QMap>
#include <QString>
#include <QList>
#include <QRectF>

class QLabel;
class QGridLayout;

class BagWidget : public QWidget {
    Q_OBJECT
  public:
    explicit BagWidget(QWidget* parent = nullptr);

    // 使用 Reference 傳遞物品與寶可夢資料
    void updateItems(QMap<QString, int>& items, QVector<PokemonItem*>& pokemons);

  signals:
    void itemUsed(const QString& itemName);

  protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // BAGWIDGET_H
