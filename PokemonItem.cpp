#include "PokemonItem.h"
#include <QtGlobal>

namespace {
struct Family {
    const char* forms[3];
    const char* moves[4];
};
const Family families[] = {
    {{"Squirtle", "Wartortle", "Blastoise"}, {"Tackle", "Tail Whip", "Protect", "Wave Crash"}},
    {{"Charmander", "Charmeleon", "Charizard"}, {"Scratch", "Growl", "Scary Face", "Flare Blitz"}},
    {{"Bulbasaur", "Ivysaur", "Venusaur"}, {"Tackle", "Growl", "Growth", "Razor Leaf"}}};
} // namespace

PokemonItem::PokemonItem(const QString& name) : PokemonItem(name, 1) {}

PokemonItem::PokemonItem(const QString& name, int level)
    : name(name), level(qMax(1, level)), maxHP(30), remainedHP(30), attack(5), defense(5) {
    for (const auto& family : families) {
        for (int stage = 0; stage < 3; ++stage) {
            if (name != QLatin1String(family.forms[stage]))
                continue;
            const int hp[] = {30, 80, 100};
            maxHP = remainedHP = hp[stage];
            attack = defense = 5 * (stage + 1);
            frontImage = QPixmap(QString(":/new/prefix1/battle/%1.png").arg(name.toLower()));
            backImage = QPixmap(QString(":/new/prefix1/battle/%1_back.png").arg(name.toLower()));
        }
    }
    skills = assignSkillsForPokemon(name, this->level);
}

PokemonItem::PokemonItem() : PokemonItem("Unknown", 1) {
    maxHP = remainedHP = 20;
}

QString PokemonItem::getName() const {
    return name;
}
int PokemonItem::getLevel() const {
    return level;
}
int PokemonItem::getMaxHP() const {
    return maxHP;
}
int PokemonItem::getRemainedHP() const {
    return remainedHP;
}
int PokemonItem::getAttack() const {
    return attack;
}
int PokemonItem::getDefense() const {
    return defense;
}
QPixmap PokemonItem::getFrontImage() const {
    return frontImage;
}
QPixmap PokemonItem::getBackImage() const {
    return backImage;
}
QVector<Skill>& PokemonItem::getSkillsRef() {
    return skills;
}
void PokemonItem::setRemainedHP(int value) {
    remainedHP = qBound(0, value, maxHP);
}
void PokemonItem::useSkillPP(int index) {
    if (index >= 0 && index < skills.size()) {
        if (skills[index].remainedPP > 0) {
            skills[index].remainedPP--;
        }
    }
}

QVector<Skill> assignSkillsForPokemon(const QString& name, int level) {
    for (const auto& family : families) {
        for (int stage = 0; stage < 3; ++stage) {
            if (name != QLatin1String(family.forms[stage]))
                continue;
            QVector<Skill> result;
            const int powers[] = {10, 15, 20, 25};
            const int pp[] = {20, 20, 15, 5};
            for (int i = 0; i < 4; ++i)
                if (level >= i + 1 && (i < 2 || stage > 0))
                    result.append(Skill(QLatin1String(family.moves[i]), powers[i], pp[i]));
            return result;
        }
    }
    return {};
}
