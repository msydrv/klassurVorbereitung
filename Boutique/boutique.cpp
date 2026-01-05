#include"boutique.h"

#include<vector>
#include<string>
#include<stdexcept>
#include<iostream>


Boutique::Boutique(std::vector<Garment> x) : offer{x}{
    if(x.begin() == x.end()) throw std::runtime_error("empty offer");
}

std::vector<Garment> Boutique::selection(const std::vector<Designer> &vd) {
    std::vector<Garment> res {};
    for(unsigned i = 0; i < this->offer.size(); i++) {
        
            if(this->offer[i].by_designer(vd) && this->offer[i].available()>0) {
                res.push_back(this->offer[i]);
            }
        
    }
    return res;
}

std::ostream& operator<<(std::ostream& o, Boutique x) {
    bool first = true;
    for(Garment &c : x.offer) {
        if(first) {
            o << "[{";
            o << c;
        }
        else {
            if(c.available() == 0) o << "*";
            o << ", " << c;
            if(c.available() == 0) o << "*";
        }
    }
    o << "]}";
    return o;
}

std::vector<Garment> Boutique::sale(Designer d) {
    std::vector<Garment> res {};
    const std::vector<Designer> des {d};
    for(unsigned i = 0 ; i <this->offer.size(); i++) {
        if(this->offer.at(i).by_designer(des) ) {
            if(this->offer.at(i).getReserve() > 0) {
                res.push_back(this->offer.at(i));
            }
            else {
                this->offer.erase(this->offer.begin() + i);
                i--;
            }

        }
    }
    return res;
}