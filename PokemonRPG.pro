QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BagWidget.cpp \
    BarrierEditor.cpp \
    BattleScene.cpp \
    BoxGenerator.cpp \
    ExplorationScene.cpp \
    MenuScene.cpp \
    PlayerItem.cpp \
    PokemonItem.cpp \
    main.cpp \
    scenemanager.cpp

HEADERS += \
    BagWidget.h \
    BarrierEditor.h \
    BattleScene.h \
    BoxGenerator.h \
    ExplorationScene.h \
    MenuScene.h \
    PlayerItem.h \
    PokemonItem.h \
    scenemanager.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Dataset/Image/NPC.png \
    Dataset/Image/bag.png \
    Dataset/Image/ball.png \
    Dataset/Image/battle/battle_scene.png \
    Dataset/Image/battle/blastoise.png \
    Dataset/Image/battle/blastoise_back.png \
    Dataset/Image/battle/bulbasaur.png \
    Dataset/Image/battle/bulbasaur_back.png \
    Dataset/Image/battle/charizard.png \
    Dataset/Image/battle/charizard_back.png \
    Dataset/Image/battle/charmander.png \
    Dataset/Image/battle/charmander_back.png \
    Dataset/Image/battle/charmeleon.png \
    Dataset/Image/battle/charmeleon_back.png \
    Dataset/Image/battle/fight_skill.png \
    Dataset/Image/battle/ivysaur.png \
    Dataset/Image/battle/ivysaur_back.png \
    Dataset/Image/battle/squirtle.png \
    Dataset/Image/battle/squirtle_back.png \
    Dataset/Image/battle/venusaur.png \
    Dataset/Image/battle/venusaur_back.png \
    Dataset/Image/battle/wartortle.png \
    Dataset/Image/battle/wartortle_back.png \
    Dataset/Image/box.png \
    Dataset/Image/dialog.png \
    Dataset/Image/icon/Ether_bag.png \
    Dataset/Image/icon/Pokeball_bag.png \
    Dataset/Image/icon/Potion_bag.png \
    Dataset/Image/item_bag_background.png \
    Dataset/Image/player/player_B.png \
    Dataset/Image/player/player_BW1.png \
    Dataset/Image/player/player_BW2.png \
    Dataset/Image/player/player_F.png \
    Dataset/Image/player/player_FW1.png \
    Dataset/Image/player/player_FW2.png \
    Dataset/Image/player/player_L.png \
    Dataset/Image/player/player_LW1.png \
    Dataset/Image/player/player_LW2.png \
    Dataset/Image/player/player_R.png \
    Dataset/Image/player/player_RW1.png \
    Dataset/Image/player/player_RW2.png \
    Dataset/Image/scene/GrassLand.png \
    Dataset/Image/scene/Town.png \
    Dataset/Image/scene/game_over.png \
    Dataset/Image/scene/lab.png \
    Dataset/Image/scene/start_menu.png

RESOURCES += \
    Dataset/Image/resources.qrc
