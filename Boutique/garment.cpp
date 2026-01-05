#include "garment.h"

#include<iostream>
#include<vector>
#include<string>
#include<stdexcept>

Garment::Garment(std::string x, int y) : description{x}, stock{y}, designer{Designer::Unknown}, reserved{} {
 if(x.empty()) throw std::runtime_error ("empty descripttion");
 if(y<0) throw std::runtime_error ("NAgative Stock");
}
    
Garment::Garment(std::string x, int y, Designer z) :  description{x}, stock{y}, designer{z}, reserved{}{
    if(x.empty()) throw std::runtime_error ("empty descripttion");
    if(y<0) throw std::runtime_error ("NAgative Stock");
}


bool Garment::by_designer(const std::vector<Designer>& vd) const {
    for(const Designer &x : vd) {
        if(x == this->designer) return true;
    }
    return false;
}

int Garment::available() const {
    return this->stock - this->reserved.size();
}

bool Garment::reserve(const std::string& name) {
    if(name.empty()) throw std::runtime_error("Empty name");
    if(this->available() == 0) return false;
    for(std::string &x : this->reserved) {
        if (x == name) return false;
    }
    this->reserved.push_back(name);
    return true;
}

std::ostream& operator<<(std::ostream &o, const Garment x ) {
    o << "[" <<  x.description << ": ";
    o << getDesigner.at(static_cast<int>(x.designer)) << ", " << x.stock << ", {" ;
    bool first = true;
    for(std::string y : x.reserved) {
        if(first) first = false;
        else {
            o << ", ";
        }
        o << y;
    }
    return o << "}]";
}

int Garment::getReserve(){
    return this->reserved.size();
}

bool Garment::buy(const std::string& name) {
    if(name.empty()) throw std::runtime_error(" ");
    for(auto it = this->reserved.begin(); it != this->reserved.end(); it++) {
        if(*it == name) {
            this->reserved.erase(it);
            this->stock -= 1;
            return true;
        }
    }
    if(this->available() > 0) {
        this->stock -= 1;
        return true;
    }
    return false;
}