#include "cabin.h"

#include<stdexcept>
#include<string>
#include<vector>

    
Cabin::Cabin(int m) : masse(m), ausstattung(Amenity::Standard) {
    if(masse < 2 || masse > 25) throw std::runtime_error("Wrong masse");
}

Cabin::Cabin(int m, Amenity a) : masse(m), ausstattung(a) {
    if(masse < 2 || masse > 25) throw std::runtime_error("Wrong masse");
}

int Cabin::get_mass() const {
    return this->masse;
}

bool Cabin::has_amenity(Amenity x) const {
    if(x == this->ausstattung) return true;
    return false;
}


std::ostream& operator<<(std::ostream& o, Cabin x) {
    o << "[mass: " << x.masse << " tons " << amenity_names[static_cast<int>(x.ausstattung)] << "-cabin]";
    return o;
}