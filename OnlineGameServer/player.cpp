 #include "player.h"
#include <unordered_map>
#include <iostream>

Player::Player(int id, int hp = 100) : id{id}, effekte{}{
    if(id <= 0) throw std::runtime_error("Player::wrong id");
    if(hp > 100 || hp < 1)throw std::runtime_error("Player::wrong id");
}

int Player::get_id() const {
    return this->id;
}

int Player::get_hp() const {
    return this->hp;
}

bool Player::has_effect(Effect e) const {
    auto it = this->effekte.find(e);
    if(this->effekte.end() == it) return false;
    return true;
}

void Player::remove_effect(Effect e) {
    if(this->effekte.find(e) != this->effekte.end()) {
        this->effekte.erase(this->effekte.find(e));
    }
}


void Player::add_effect(Effect e, int duration) {
    if(duration <= 0) throw std::runtime_error("Player::wrong id");
    this->effekte[e] = duration;
}


void Player::tick() {
        if(this->effekte.find(Effect::Poison) != this->effekte.end()) {
        this->hp = this->hp-5;
        if(this->hp<0) this->hp = 0;
    }
    for(auto it = this->effekte.begin(); it !=this->effekte.end();) {
        (*it).second--;
        if((*it).second <= 0) it = effekte.erase(it);
        else it++;
    }

}



int Player::get_size() {
    return this->effekte.size();
}

std::ostream& operator<<(std::ostream& o, Player x) {
    o << " - Player " << x.id << " (hp: " << x.hp << ") [";
    std::vector<std::string> aus {"Poison","Shield","Speed","Invisibility","Regeneration"};
    for(const auto &i : x.effekte) {
        o << aus[static_cast<int>(i.first)] << "(" << i.second << ")"; 
    } 
    return o << "]";
}
