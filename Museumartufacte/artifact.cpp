#include"artifact.h"

#include<iostream>
#include<vector>
#include<string>
#include<stdexcept>


Artifact::Artifact(int x) : level{x}, tag{Tag::Relic} {
    if(level > 9 || level < 1) throw std::runtime_error("Artifact::Invalid Level");
}
    
Artifact::Artifact(int x, Tag y) : level{x}, tag{y} {
    if(level > 9 || level < 1) throw std::runtime_error("Artifact::Invalid Level");
}


int Artifact::get_level() const {
    return this->level;
}

bool Artifact::has_tag(Tag x) const {
    if(this->tag == x) return true;
    return false;
}


std::ostream& operator<<(std::ostream& o, Artifact x) {
    return o << "[lvl: " << x.get_level() << " " << tag_names[static_cast<int>(x.tag)] << "-artifact]";
}