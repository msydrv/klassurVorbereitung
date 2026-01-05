#ifndef SMARTCITY_H
#define SMARTCITY_H

#include "sensor.h"

class SmartCity {
    std::unordered_map<int, Sensor> sensors;
public:
    void add_sensor(const Sensor&);
    void remove_sensor(int id);


};


#endif