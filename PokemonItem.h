#ifndef POKEMONITEM_H
#define POKEMONITEM_H

#include <QString>
#include <QPixmap>
#include <QVector>

struct Skill {
    QString name;
    int power;
    int pp;
    int remainedPP;

    Skill(const QString& name, int power, int pp)
        : name(name), power(power), pp(pp), remainedPP(pp) {}
    Skill() : name(""), power(0), pp(0), remainedPP(0) {}
};

class PokemonItem {
  public:
    PokemonItem(const QString& name);
    PokemonItem(const QString& name, int level);
    PokemonItem();
    QString getName() const;
    int getLevel() const;
    int getMaxHP() const;
    int getRemainedHP() const;
    int getAttack() const;
    int getDefense() const;
    QVector<Skill>& getSkillsRef();
    QPixmap getFrontImage() const;
    QPixmap getBackImage() const;
    void setRemainedHP(int value);
    void useSkillPP(int index);

  private:
    QString name;
    int level;
    int maxHP;
    int remainedHP;
    int attack;
    int defense;
    QVector<Skill> skills;
    QPixmap frontImage;
    QPixmap backImage;
};

QVector<Skill> assignSkillsForPokemon(const QString& name, int level);
#endif // POKEMONITEM_H
