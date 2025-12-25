#ifndef GAMESERVER_H
#define GAMESERVER_H

#include"player.h"

#include<unordered_map>

class GameServer {
    std::unordered_map <int, Player> players;
public:
    void add_player(const Player&);
    void remove_player(int id);
    void apply_round();

    std::unordered_map<Effect,int> effect_statistics() const;

    void cleanse(Effect e);

    Player& most_affected_player();

    friend std::ostream& operator<<(std::ostream& o, GameServer x);
};
std::ostream& operator<<(std::ostream& o, GameServer x);

#endif
