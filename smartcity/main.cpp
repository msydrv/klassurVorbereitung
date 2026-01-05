// main.cpp
// Tests für "Smart City – Sensoren & Messwerte" (unordered_map + enum + iterator + lambdas)
//
// Erwartete Schnittstellen (wie in der Aufgabe):
//
// enum class SensorType { Temperature, Humidity, CO2, Noise, Light };
//
// class Sensor {
// public:
//   Sensor(int id, SensorType type, double value);
//   int get_id() const;
//   SensorType get_type() const;
//   double get_value() const;
//   void set_value(double v);
// };
//
// class SmartCity {
// public:
//   void add_sensor(const Sensor&);
//   void remove_sensor(int id);
//
//   bool any_sensor_above(SensorType type, double limit) const;   // find_if + lambda
//   int count_sensors_if(std::function<bool(const Sensor&)> pred) const;
//   Sensor& find_first(std::function<bool(const Sensor&)> pred);
//   void remove_all_if(std::function<bool(const Sensor&)> pred); // iterator+erase
//   double average_value_of(SensorType type) const;
//
//   std::function<bool(const Sensor&)> make_threshold_filter(SensorType t, double limit); // Zusatz 1
//   SensorType most_critical_type() const; // Zusatz 2
// };
//
// WICHTIG: Passe den Include unten an deinen Dateinamen an.

#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <functional>

using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

// >>> ANPASSEN <<<
#include "smartCity.h"

// ---------------- Mini-Testframework ----------------
static int g_tests = 0;
static int g_failed = 0;

void check(bool cond, const string& msg) {
    ++g_tests;
    if (!cond) {
        ++g_failed;
        cout << "[FAIL] " << msg << endl;
    } else {
        cout << "[ OK ] " << msg << endl;
    }
}

template <typename F>
void expect_throw(F f, const string& msg) {
    ++g_tests;
    try {
        f();
        ++g_failed;
        cout << "[FAIL] " << msg << " (no exception)" << endl;
    } catch (const runtime_error&) {
        cout << "[ OK ] " << msg << " (runtime_error)" << endl;
    } catch (...) {
        ++g_failed;
        cout << "[FAIL] " << msg << " (wrong exception type)" << endl;
    }
}

int main() {
    cout << "==== PR1 Tests: Smart City ====\n" << endl;

    // ---------------- Sensor: ctor validation ----------------
    expect_throw([](){ Sensor s(0, SensorType::Temperature, 10.0); }, "Sensor id must be > 0");
    expect_throw([](){ Sensor s(1, SensorType::CO2, -1.0); }, "Sensor value must be >= 0");

    {
        Sensor s(7, SensorType::Light, 50.5);
        check(s.get_id() == 7, "Sensor get_id()");
        check(s.get_type() == SensorType::Light, "Sensor get_type()");
        check(s.get_value() == 50.5, "Sensor get_value()");
        s.set_value(0.0);
        check(s.get_value() == 0.0, "Sensor set_value()");
        expect_throw([&](){ s.set_value(-0.1); }, "Sensor set_value negative throws");
    }

    // ---------------- SmartCity: add/remove ----------------
    {
        SmartCity city;
        Sensor a(1, SensorType::Temperature, 18.0);
        Sensor b(2, SensorType::Noise, 45.0);

        city.add_sensor(a);
        city.add_sensor(b);

        expect_throw([&](){ city.add_sensor(a); }, "add_sensor duplicate id throws");

        city.remove_sensor(2);
        expect_throw([&](){ city.remove_sensor(2); }, "remove_sensor unknown id throws");
    }

    // Prepare a common city
    SmartCity city;
    city.add_sensor(Sensor(1, SensorType::Temperature, 22.0));
    city.add_sensor(Sensor(2, SensorType::Temperature, 35.0));
    city.add_sensor(Sensor(3, SensorType::Humidity, 60.0));
    city.add_sensor(Sensor(4, SensorType::CO2, 900.0));
    city.add_sensor(Sensor(5, SensorType::Noise, 70.0));
    city.add_sensor(Sensor(6, SensorType::Noise, 85.0));
    city.add_sensor(Sensor(7, SensorType::Light, 300.0));

    // ---------------- any_sensor_above (find_if + lambda) ----------------
    check(city.any_sensor_above(SensorType::Temperature, 30.0), "any_sensor_above: temp > 30 exists");
    check(!city.any_sensor_above(SensorType::Humidity, 80.0), "any_sensor_above: humidity > 80 does not exist");
    check(city.any_sensor_above(SensorType::CO2, 500.0), "any_sensor_above: CO2 > 500 exists");

    // ---------------- count_sensors_if (lambda passed in) ----------------
    {
        int loud = city.count_sensors_if([](const Sensor& s){
            return s.get_type() == SensorType::Noise && s.get_value() > 80.0;
        });
        check(loud == 1, "count_sensors_if: noise > 80 count");

        int lowValue = city.count_sensors_if([](const Sensor& s){
            return s.get_value() < 25.0;
        });
        check(lowValue == 1, "count_sensors_if: value < 25 count");
    }

    // ---------------- find_first (returns reference) ----------------
    {
        Sensor& s = city.find_first([](const Sensor& x){
            return x.get_type() == SensorType::Noise && x.get_value() > 80.0;
        });
        check(s.get_id() == 6, "find_first returns correct sensor (id=6)");

        // reference test: modify through reference
        s.set_value(79.0);
        check(!city.any_sensor_above(SensorType::Noise, 80.0),
              "find_first returns reference (modification affects city)");

        // restore
        s.set_value(85.0);

        expect_throw([&](){
            (void)city.find_first([](const Sensor& x){
                return x.get_type() == SensorType::Noise && x.get_value() > 1000.0;
            });
        }, "find_first throws if none match");
    }

    // ---------------- remove_all_if (iterator+erase) ----------------
    {
        // remove all Noise sensors
        city.remove_all_if([](const Sensor& s){
            return s.get_type() == SensorType::Noise;
        });

        check(!city.any_sensor_above(SensorType::Noise, 0.0), "remove_all_if removed all Noise sensors");
        int remainingNoise = city.count_sensors_if([](const Sensor& s){
            return s.get_type() == SensorType::Noise;
        });
        check(remainingNoise == 0, "count noise after remove_all_if");

        // remove all high CO2 sensors (> 800)
        city.remove_all_if([](const Sensor& s){
            return s.get_type() == SensorType::CO2 && s.get_value() > 800.0;
        });
        check(!city.any_sensor_above(SensorType::CO2, 0.0), "remove_all_if removed CO2 sensor");
    }

    // ---------------- average_value_of ----------------
    {
        // current city has Temperature(22,35), Humidity(60), Light(300)
        double avgTemp = city.average_value_of(SensorType::Temperature);
        check(avgTemp == (22.0 + 35.0) / 2.0, "average_value_of Temperature correct");

        double avgHumidity = city.average_value_of(SensorType::Humidity);
        check(avgHumidity == 60.0, "average_value_of Humidity correct");

        expect_throw([&](){ (void)city.average_value_of(SensorType::Noise); }, "average_value_of throws if none exist");
    }

    // ---------------- make_threshold_filter (returns lambda) ----------------
    {
        auto hot = city.make_threshold_filter(SensorType::Temperature, 30.0);
        int hotCount = city.count_sensors_if(hot);
        check(hotCount == 1, "make_threshold_filter works with count_sensors_if");

        // remove the hot temperature sensor
        city.remove_all_if(hot);
        check(!city.any_sensor_above(SensorType::Temperature, 30.0), "make_threshold_filter works with remove_all_if");
    }

    // ---------------- most_critical_type ----------------
    {
        // After removals: Temperature(22), Humidity(60), Light(300)
        // Max per type: Temp=22, Humidity=60, Light=300 -> most critical = Light
        SensorType t = city.most_critical_type();
        check(t == SensorType::Light, "most_critical_type returns type with highest max value");
    }

    // ---------------- most_critical_type throws if empty ----------------
    {
        SmartCity empty;
        expect_throw([&](){ (void)empty.most_critical_type(); }, "most_critical_type throws if no sensors");
    }

    cout << "\n==== SUMMARY ====\n";
    cout << "Tests:  " << g_tests << "\n";
    cout << "Failed: " << g_failed << "\n";
    cout << (g_failed == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");

    return (g_failed == 0) ? 0 : 1;
}