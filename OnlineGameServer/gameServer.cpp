#include"gameServer.h"

#include<vector>
#include<iostream>
    
void GameServer::add_player(const Player& x) {
    if(this->players.find(x.get_id()) != this->players.end()) throw std::runtime_error("GameServer::Add_Player");
    this->players.insert(std::pair<int, Player>{x.get_id(), x});
}


void GameServer::remove_player(int id) {
    if(this->players.find(id) == this->players.end()) throw std::runtime_error("GameServer::remove_player");
    this->players.erase(this->players.find(id));
}
    
void GameServer::apply_round() {
    for(auto it = this->players.begin(); it != this->players.end();) {
        (*it).second.tick();
        if((*it).second.get_hp() == 0) it = players.erase(it);
        else it++;
    }
}


std::unordered_map<Effect,int> GameServer::effect_statistics() const {
    std::vector<int> count(5, 0);
    for(const auto& i : this->players) {
        if(i.second.has_effect(Effect::Poison)) count[0]++;
        if(i.second.has_effect(Effect::Shield)) count[1]++;
        if(i.second.has_effect(Effect::Speed)) count[2]++;
        if(i.second.has_effect(Effect::Invisibility)) count[3]++;
        if(i.second.has_effect(Effect::Regeneration)) count[4]++;
    }
    std::vector<std::string> aus {"Poison","Shield","Speed","Invisibility","Regeneration"};
    std::unordered_map<Effect, int> res;
    res.insert(std::pair<Effect, int> {Effect::Poison ,count[0]});
    res.insert(std::pair<Effect, int> {Effect::Shield ,count[1]});
    res.insert(std::pair<Effect, int> {Effect::Speed ,count[2]});
    res.insert(std::pair<Effect, int> {Effect::Invisibility ,count[3]});
    res.insert(std::pair<Effect, int> {Effect::Regeneration ,count[4]});
    return res;
}

std::ostream& operator<<(std::ostream& o, GameServer x) {
    o << "Server with " << x.players.size() << "players:";
    for(auto &i : x.players) {
        o << i.second << std::endl; 
    }
    return o;
}

void GameServer::cleanse(Effect e) {
    bool was = true;
    for(auto &c : this->players) {
        if(c.second.has_effect(e)) {
            c.second.remove_effect(e);
            was = false;
        }
    }
    if(was) throw std::runtime_error("");
}

Player& GameServer::most_affected_player() {
    int most = 0, res = 0; 
    int i;
    for(int i = 0; i < this->players.size(); i++) {
        int tmp = this->players.at(i).get_size();
        if(tmp >most) {
            most = tmp;
            res = i;
        }
    }
    Player & erg = this->players.at(i);
    return erg;
}