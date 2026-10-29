// --- BarrierEditor.cpp ---
#include "BarrierEditor.h"
#include <QGraphicsTextItem>
#include <QPen>

BarrierOverlay::BarrierOverlay(QGraphicsScene* scene) : scene(scene) {}

void BarrierOverlay::drawBarriers(const QString& mapName) {
    QList<QRect> barriers;
    QList<QRect> ledges;
    QList<QRect> signs;
    QList<QPair<QRect, QString>> portals;

    if (mapName == "lab") {
        barriers = {{300, 365, 30, 100}, {560, 405, 145, 80}, {300, 300, 455, 65},
                    {300, 520, 195, 90}, {560, 520, 195, 90}, {320, 385, 105, 105},
                    {300, 650, 55, 88},  {698, 650, 57, 88},  {300, 300, 15, 438},
                    {735, 300, 20, 438}, {300, 738, 455, 5}};
        portals = {{{505, 728, 55, 10}, "toTownFromLab"}};
    } else if (mapName == "Town") {
        barriers = {
            {0, 0, 515, 90},      {0, 930, 1000, 70},  {190, 150, 250, 230}, {560, 150, 250, 230},
            {520, 450, 335, 235}, {0, 0, 80, 1000},    {190, 525, 245, 55},  {525, 780, 285, 55},
            {270, 825, 205, 175}, {900, 0, 100, 1000}, {565, 0, 435, 90},    {530, 305, 30, 75},
            {150, 305, 40, 75},   {195, 670, 70, 75},  {375, 524, 60, 58},   {670, 780, 60, 55}};
        signs = {{195, 670, 70, 75}, {375, 524, 60, 58}, {670, 780, 60, 55}};
        portals = {{{670, 686, 35, 5}, "toLab"}, {{515, 0, 50, 5}, "toGrassland"}};
    } else if (mapName == "GrassLand") {
        barriers = {{0, 0, 1000, 70},      {0, 0, 100, 1667},     {900, 0, 100, 1667},
                    {100, 1480, 415, 187}, {565, 1480, 335, 187}, {310, 100, 120, 380},
                    {400, 565, 285, 125},  {100, 565, 85, 125},   {100, 980, 415, 125},
                    {355, 1265, 80, 55},   {355, 1265, 80, 55}};
        ledges = {{100, 205, 585, 40},  {100, 410, 210, 40},  {185, 615, 215, 40},
                  {100, 825, 70, 40},   {195, 825, 195, 40},  {440, 825, 600, 40},
                  {730, 1075, 170, 40}, {100, 1290, 170, 40}, {435, 1290, 465, 40}};
        for (const QRect& ledge : ledges) {
            barriers.append(QRect(ledge.left(), ledge.top(), 5, ledge.height()));
            barriers.append(QRect(ledge.right() - 5, ledge.top(), 5, ledge.height()));
        }
        signs = {{355, 1265, 80, 55}};
        portals = {{{515, 1640, 50, 30}, "toTownFromGrass"}};
    } else {
        return;
    }

    for (const QRect& r : barriers) {
        auto* b = scene->addRect(r, QPen(Qt::NoPen));
        b->setData(0, "barrier");
    }

    for (const QRect& r : ledges) {
        auto* l = scene->addRect(r, QPen(Qt::NoPen));
        l->setData(0, "ledge");
    }

    for (const QRect& r : signs) {
        auto* s = scene->addRect(r, QPen(Qt::NoPen));
        s->setData(0, "sign");
    }

    for (const auto& pair : portals) {
        auto* p = scene->addRect(pair.first, QPen(Qt::NoPen));
        p->setData(0, "portal");
        p->setData(1, pair.second);
    }
}
