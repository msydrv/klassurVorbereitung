#include "cableCatLine.h"
#include<iostream>
#include<vector>
#include<string>
#include<stdexcept>


CableCarLine::CableCarLine(int a , int m , int mk) : antriebMasse{a}, maximalLast{m}, mKabinenAnzahl{mk}, cabinenListe{}  {
    if(a<40 || a>180) throw std::runtime_error("Wrong Antriebmasse");
    if(m<120 || m>8000) throw std::runtime_error("Wrong MaximalLast");
    if(mk<1 || mk>200) throw std::runtime_error("Wrong KabinenAnzahl");
}


CableCarLine::CableCarLine(int a, int m , int mk, std::vector<Cabin> h) : antriebMasse{a}, maximalLast{m}, mKabinenAnzahl{mk}, cabinenListe{h} {
    if(a<40 || a>180) throw std::runtime_error("Wrong Antriebmasse");
    if(m<120 || m>8000) throw std::runtime_error("Wrong MaximalLast");
    if(mk<1 || mk>200) throw std::runtime_error("Wrong KabinenAnzahl");
    if(h.size() > mk) throw std::runtime_error("Wrong KabinenAnzahl");
}

int CableCarLine::total_load() const {
    int res = 0;
    for(const auto& x : this->cabinenListe) {
        res += x.get_mass();
    }
    return res + this->antriebMasse;
}

bool CableCarLine::operational() const {
    if(this->total_load() > this->maximalLast) return false;
    std::vector<bool> i(5, false);
    int nAid = 0;
    for(unsigned c = 0; c < this->cabinenListe.size(); c++) {
        if(this->cabinenListe.at(c).has_amenity(Amenity::Heater)) {
            i[0] = true;
            nAid++;
        }
        else if(this->cabinenListe.at(c).has_amenity(Amenity::Panorama)) {
            i[1] = true;
            nAid++;
        }    
        else if(this->cabinenListe.at(c).has_amenity(Amenity::BikeRack)) {
            if(!this->cabinenListe.at(c+1).has_amenity(Amenity::Heater)) return false;
            i[2] = true;
            nAid++;
        }
        else if(this->cabinenListe.at(c).has_amenity(Amenity::Standard)) {
            i[3] = true;
            nAid++;
        }
        else if(this->cabinenListe.at(c).has_amenity(Amenity::FirstAid)) {
            i[4] = true;
            nAid = 0;
        }
        if(nAid>2) return false;
    }
    for(const auto &c : i) {
        if(!c) return false;
    }
    return true;
}

void CableCarLine::attach(std::vector<Cabin> x) {
    if(x.size() + this->cabinenListe.size() > this->mKabinenAnzahl) {
        throw std::runtime_error("maximale Cabinen Anzahl überschritten");
    }
    for(const auto &c : x) {
        this->cabinenListe.push_back(c);
    }
}

std::vector<Cabin> CableCarLine::detach(size_t from) {

    std::vector<Cabin> res(
        cabinenListe.begin() + from,
        cabinenListe.end()
    );

    cabinenListe.erase(
        cabinenListe.begin() + from,
        cabinenListe.end()
    );

    return res;
}


std::ostream& operator<<(std::ostream& o , CableCarLine x) {
    o << "[" << x.total_load() << "/" << x.maximalLast << " tons, ";
    if(x.operational()) o << "operational, ";
    else o << "not operational, ";
    o << "max: " << x.mKabinenAnzahl << " cabins [ ";
    for(const auto &c : x.cabinenListe) {
        o << " " << c << " "; 
    }
    o << "].";
    return o;
}