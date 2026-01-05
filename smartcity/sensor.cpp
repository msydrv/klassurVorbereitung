#include "sensor.h"

#include<string>
#include<vector>

Sensor::Sensor(int id, SensorType type, double value) : ID{id} , type{type}, messWert{value} {
    if (id<=0) throw std::runtime_error("Sensor::Sensor::wrong id");
    if(messWert< 0) throw std::runtime_error("Sensor::Sensor::wrong messWert");
}

int Sensor::get_id() const {
    return this->ID;
}

SensorType Sensor::get_type() const {
    return this->type;
}
double Sensor::get_value() const {
    return this->messWert;
}

void Sensor::set_value(double v) {
    if(v<0) throw std::runtime_error("Sensor::set_value imposible new v");
    this->messWert = v;
}