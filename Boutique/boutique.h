#ifndef BOUTIQUE_H
#define BOUTIQUE_H

#include"garment.h"

#include<vector>

class Boutique {
    std::vector<Garment> offer;
public:
    Boutique(std::vector<Garment>);

    std::vector<Garment> selection(const std::vector<Designer> &vd); 

    std::vector<Garment> sale(Designer d); 

    friend std::ostream& operator<<(std::ostream&, Boutique);
};

std::ostream& operator<<(std::ostream&, Boutique);

#endif