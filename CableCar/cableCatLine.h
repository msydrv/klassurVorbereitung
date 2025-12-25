#ifndef CABLECATLINE_H
#define CABLECATLINE_H


#include "cabin.h"

#include <vector>
#include<iostream>


class CableCarLine {
    int antriebMasse, maximalLast, mKabinenAnzahl;
    std::vector<Cabin> cabinenListe;
public:
    CableCarLine(int, int, int);
    CableCarLine(int, int, int, std::vector<Cabin>);

    int total_load() const;

    bool operational() const;

    void attach(std::vector<Cabin>);

    std::vector<Cabin> detach(size_t from);

    friend std::ostream& operator<<(std::ostream&, CableCarLine);
};

std::ostream& operator<<(std::ostream&, CableCarLine);

#endif