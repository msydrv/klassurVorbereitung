#ifndef SENSOR_H
#define SENSOR_H

#include<iostream>
#include<vector>
#include<string>


enum class SensorType {
    Temperature,
    Humidity,
    CO2,
    Noise,
    Light
};

std::vector<std::string> sensorTypePrint{
    "Temperature",
    "Humidity",
    "CO2",
    "Noise",
    "Light"    
};



class Sensor {
    int ID; // >0
    SensorType type; 
    double messWert; // >= 0
public:
    Sensor(int id, SensorType type, double value);

    int get_id() const;
    SensorType get_type() const;
    double get_value() const;

    void set_value(double v);

};


#endif