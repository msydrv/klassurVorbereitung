#ifndef CABIN_H
#define CABIN_H


#include<iostream>
#include<vector>
#include<string>



enum class Amenity { Heater, Panorama, BikeRack, Standard, FirstAid };
const std::vector<std::string> amenity_names {"Heater", "Panorama", "BikeRack", "Standard", "FirstAid"};


class Cabin{
    int masse;
    Amenity ausstattung;
public:
    Cabin(int);
    Cabin(int, Amenity);

    int get_mass() const;

    bool has_amenity(Amenity) const;

    friend std::ostream& operator<<(std::ostream& o, Cabin x);


};

std::ostream& operator<<(std::ostream& o, Cabin x);



#endif // CABIN_H