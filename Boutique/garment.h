#ifndef GARMENT_H
#define GARMENT_H

#include<string>
#include<stdexcept>
#include<vector>

const enum class Designer{Prada, Chanel, Dior, Unknown};
const std::vector<std::string> getDesigner{"Prada", "Chanel", "Dior", "Unknown"};


class Garment{
    std::string description; // nicht leer
    int stock; // nicht negativ
    Designer designer; // default Unknown 
    std::vector<std::string> reserved; // leer
public:   
    Garment(std::string, int);
    Garment(std::string, int, Designer);


    bool by_designer(const std::vector<Designer>& vd) const;

    int available() const;

    bool reserve(const std::string& name);

    friend std::ostream& operator<<(std::ostream &o, Garment x );

    bool buy(const std::string& name);

    int getReserve();
};

std::ostream& operator<<(std::ostream &o, const Garment x);


#endif

