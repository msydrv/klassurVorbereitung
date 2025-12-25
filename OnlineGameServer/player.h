#ifndef PLAYER_H
#define PLAYER_H

#include<unordered_map>

enum class Effect {
    Poison,
    Shield,
    Speed,
    Invisibility,
    Regeneration
};


class Player {
    int id;
    int hp;
    std::unordered_map<Effect, int> effekte {};
public:
    Player(int id, int hp);

    int get_id() const;
    int get_hp() const;
    bool has_effect(Effect e) const;
    void add_effect(Effect e, int duration);
    void tick();
    int get_size();

    friend std::ostream& operator<<(std::ostream& o, Player x);

    void remove_effect(Effect e);
};

std::ostream& operator<<(std::ostream& o, Player x);

#endif