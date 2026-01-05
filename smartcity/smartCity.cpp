#include"smartCity.h"

void SmartCity::add_sensor(const Sensor& c) {
    if(this->sensors.find(c.get_id()) != this->sensors.end()) throw std::runtime_error("smartCity::add_Sensor::taken id");
    this->sensors.insert(std::pair<int, Sensor>(c.get_id(), c));
}
    
void SmartCity::remove_sensor(int id) {
    if(this->sensors.find(id) == this->sensors.end()) throw std::runtime_error("smart::remove_sensor:: id doesnt exist");
    this->sensors.erase(this->sensors.find(id));
}


