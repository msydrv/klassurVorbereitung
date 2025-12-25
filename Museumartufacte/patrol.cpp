#include"patrol.h"

Patrol::Patrol(int a, int b) : fartigue_limit(a), risk_cap(b), list{}{
    if(fartigue_limit<10 || fartigue_limit > 200) throw std::runtime_error("Patrol::Wrong fartigue level");
    if(risk_cap<15 || risk_cap > 5000) throw std::runtime_error("Patrol::Wrong risk cap");    
    
}


Patrol::Patrol(int a, int b, std::vector<Artifact> c) : fartigue_limit{a}, risk_cap{b}, list{c} {
    if(fartigue_limit<10 || fartigue_limit > 200) throw std::runtime_error("Patrol::Wrong fartigue level");
    if(risk_cap<15 || risk_cap > 5000) throw std::runtime_error("Patrol::Wrong risk cap");        
    int sum = 0;
    for(const auto &x : list) {
        sum += x.get_level();
    }
    if(sum > fartigue_limit) throw std::runtime_error("Patrol::Wrong risk 1"); ;
}

int Patrol::total_risk() const {
    int res;
    for(const auto &x : this->list) {
        res = res + (x.get_level() * x.get_level());
    }
    return res;
}

bool Patrol::approved() const {
    if(this->total_risk() > this->risk_cap) return false;
    std::vector<bool> x (5, false);
    bool störung = false;
    for(auto it = this->list.begin(); it != this->list.end();) {
        if((*it).has_tag(Tag::Fragile)) {
            x[0] = true;
            if(it != this->list.begin() || it != this->list.end()--) {
                it--;
                int a = (*it).get_level();
                it++;
                it++;
                int b = (*it).get_level();
                if(a+b>10) return false;
                it--;
            }
            else {
            }
        }   else if((*it).has_tag(Tag::Painting)) {
            x[1] = true;
            störung = false;
        }   else if((*it).has_tag(Tag::Sculpture)) {
            x[2] = true;
        }   else if((*it).has_tag(Tag::Digital)) {
            x[3] = true;
            störung = true;
        }   else if((*it).has_tag(Tag::Relic)) {
            x[4] = true;
            it++;
            if((*it).has_tag(Tag::Relic)) {
                return false;   
            }
            it--;
        }
        it++;
    }
    for(const auto &j : x) {
        if(j == false) return false;
    }
    if(störung) return false;
    return true;
}


void Patrol::add(std::vector<Artifact>x) {
    int current = 0;
    for (const auto& a : list)
        current += a.get_level();

    int added = 0;
    for (const auto& a : x)
        added += a.get_level();

    if (current + added > this->fartigue_limit)
        throw std::runtime_error("Patrol::add::fatigue limit exceeded");

    list.insert(list.end(), x.begin(), x.end());
}
/*
void Patrol::add(std::vector<Artifact>x) {
    this->list.insert(this->list.end(), x.begin(), x.end());
    if(this->total_risk()> this->fartigue_limit) throw std::runtime_error("Patrol::add::Wrong size");
}
*/
std::vector<Artifact> Patrol::cut(size_t from) {
    if(from > this->list.size()-1) throw std::runtime_error("Patrol::cut::Wrong from");
    std::vector<Artifact> res(this->list.begin() + from, this->list.end());
    this->list.erase(this->list.begin() + from, this->list.end());
    return res;
}

std::ostream& operator<<(std::ostream& o, Patrol x) {
    o << "[risk " << x.total_risk() << "/" << x.risk_cap << ", fatigue " << x.list.size() << "/" << x.fartigue_limit << ", ";
    if(x.approved()) o << "approved {"; 
    else o << "not approved { ";
    for(const auto &c : x.list) {
        o << c;
    }
    return o << " }";
}


void Patrol::stable_group() {
    std::vector<Artifact> result;

    // Fragile
    for (const auto& a : list)
        if (a.has_tag(Tag::Fragile))
            result.push_back(a);

    // Painting
    for (const auto& a : list)
        if (a.has_tag(Tag::Painting))
            result.push_back(a);

    // Sculpture
    for (const auto& a : list)
        if (a.has_tag(Tag::Sculpture))
            result.push_back(a);

    // Digital
    for (const auto& a : list)
        if (a.has_tag(Tag::Digital))
            result.push_back(a);

    // Relic
    for (const auto& a : list)
        if (a.has_tag(Tag::Relic))
            result.push_back(a);

    this->list = result;
}


    void splice(size_t from, size_t count, Patrol& to, size_t pos);