// main.cpp
// Test-Programm für die Probeklausur "Seilbahn (Cabin / CableCarLine)"
//
// Voraussetzung:
// - Du hast die Klassen/Enums genau wie in der Angabe implementiert:
//   enum class Amenity { Heater, Panorama, BikeRack, Standard, FirstAid };
//   class Cabin { ... };
//   class CableCarLine { ... };
// - operator<< für Cabin und CableCarLine existiert.
// - Methoden: get_mass(), has_amenity(), total_load(), operational(),
//   attach(...), detach(...), group_by_amenity(), reroute(...)

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::runtime_error;

// >>> Passe diesen Include an dein Projekt an (z.B. "seilbahn.h" / "CableCar.h" etc.

#include"cableCatLine.h"

// ---------- Mini-Testframework ----------
static int g_tests = 0;
static int g_failed = 0;

void check(bool cond, const std::string& msg) {
    ++g_tests;
    if (!cond) {
        ++g_failed;
        cout << "[FAIL] " << msg << endl;
    } else {
        cout << "[ OK ] " << msg << endl;
    }
}

template <typename F>
void expect_throw(F f, const std::string& msg) {
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

// ---------- Hilfsfunktionen ----------
Cabin C(int mass, Amenity a) { return Cabin(mass, a); }

// Erzeugt eine betriebsbereite Linie (falls Implementierung korrekt)
// Regeln:
// 1) total_load <= max_load
// 2) jede Amenity mindestens 1x
// 3) nie mehr als 2 in Folge ohne FirstAid
// 4) wenn BikeRack vorkommt, muss danach mind. ein Heater kommen
CableCarLine make_operational_line() {
    int drive = 60;
    int maxLoad = 500;
    size_t maxCabins = 20;

    // Reihenfolge so, dass:
    // - es gibt FirstAid oft genug, sodass "ohne FirstAid" nie > 2 hintereinander ist
    // - BikeRack kommt vor Heater, damit Regel (4) erfüllt ist
    vector<Cabin> cabins = {
        C(7,  Amenity::Standard),
        C(8,  Amenity::FirstAid),   // breaks "no FirstAid"-runs
        C(9,  Amenity::Panorama),
        C(10, Amenity::FirstAid),
        C(11, Amenity::BikeRack),   // BikeRack
        C(12, Amenity::FirstAid),
        C(13, Amenity::Heater)      // Heater AFTER BikeRack -> rule (4) satisfied
    };

    return CableCarLine(drive, maxLoad, maxCabins, cabins);
}

int main() {
    cout << "==== PR1 Probeklausur Tests: Seilbahn ====\n" << endl;

    // --------- Cabin Konstruktor/Validierung ----------
    expect_throw([]() { Cabin bad1(1); }, "Cabin mass < 2 throws");
    expect_throw([]() { Cabin bad2(26, Amenity::Standard); }, "Cabin mass > 25 throws");

    {
        Cabin a(7); // default Standard
        check(a.get_mass() == 7, "Cabin get_mass()");
        check(a.has_amenity(Amenity::Standard), "Cabin default amenity is Standard");
        check(!a.has_amenity(Amenity::Heater), "Cabin has_amenity() false case");
        cout << "Cabin print: " << a << endl;
    }

    // --------- CableCarLine Konstruktor/Validierung ----------
    expect_throw([]() { CableCarLine x(39, 300, 10); }, "Drive mass < 40 throws");
    expect_throw([]() { CableCarLine x(60, 119, 10); }, "Max load < 120 throws");
    expect_throw([]() { CableCarLine x(60, 300, 0); }, "Max cabins < 1 throws");

    {
        vector<Cabin> tooMany;
        for (int i = 0; i < 5; ++i) tooMany.push_back(C(7, Amenity::Standard));
        expect_throw([&]() { CableCarLine x(60, 300, 4, tooMany); }, "Initial cabin list > maxCabins throws");
    }

    // --------- total_load ----------
    {
        auto line = make_operational_line();
        int expected = 60;
        // 7+8+9+10+11+12+13 = 70
        expected += 70;
        check(line.total_load() == expected, "CableCarLine total_load()");
        cout << "Line print: " << line << endl;
    }

    // --------- operational(): positive case ----------
    {
        auto line = make_operational_line();
        check(line.operational(), "operational() true for valid configuration");
    }

    // --------- operational(): Regel (2) missing amenity ----------
    {
        int drive = 60, maxLoad = 500;
        size_t maxCabins = 20;
        vector<Cabin> cabins = {
            C(7, Amenity::Standard),
            C(8, Amenity::FirstAid),
            C(9, Amenity::Panorama),
            C(10, Amenity::FirstAid),
            // missing Heater + BikeRack
            C(11, Amenity::Standard)
        };
        CableCarLine line(drive, maxLoad, maxCabins, cabins);
        check(!line.operational(), "operational() false if not all amenities exist");
    }

    // --------- operational(): Regel (3) >2 ohne FirstAid hintereinander ----------
    {
        int drive = 60, maxLoad = 500;
        size_t maxCabins = 30;
        vector<Cabin> cabins = {
            C(7, Amenity::Heater),
            C(7, Amenity::Panorama),
            C(7, Amenity::BikeRack),   // 3 in a row without FirstAid -> violation
            C(7, Amenity::Standard),
            C(7, Amenity::FirstAid)
        };
        // Enthält zwar alle amenities? Nein (FirstAid 1x, Heater, Panorama, BikeRack, Standard) -> ja, alle da.
        CableCarLine line(drive, maxLoad, maxCabins, cabins);
        check(!line.operational(), "operational() false for >2 consecutive without FirstAid");
    }

    // --------- operational(): Regel (4) BikeRack aber kein Heater danach ----------
    {
        int drive = 60, maxLoad = 500;
        size_t maxCabins = 30;
        vector<Cabin> cabins = {
            C(7,  Amenity::Heater),    // Heater BEFORE
            C(8,  Amenity::FirstAid),
            C(9,  Amenity::BikeRack),  // BikeRack appears
            C(10, Amenity::Panorama),
            C(11, Amenity::Standard),
            C(12, Amenity::FirstAid)
            // no Heater AFTER BikeRack -> violation
        };
        // Alle amenities vorhanden? Heater, Panorama, BikeRack, Standard, FirstAid -> ja.
        CableCarLine line(drive, maxLoad, maxCabins, cabins);
        check(!line.operational(), "operational() false if BikeRack exists but no Heater after it");
    }

    // --------- attach(): capacity + no state change on failure ----------
    {
        // maxCabins = 3, initial 2 cabins, attach 2 -> should throw and keep original
        int drive = 60, maxLoad = 500;
        size_t maxCabins = 3;
        vector<Cabin> cabins = { C(7, Amenity::Standard), C(8, Amenity::FirstAid) };
        CableCarLine line(drive, maxLoad, maxCabins, cabins);

        cout << "Before attach fail: " << line << endl;

        vector<Cabin> add = { C(9, Amenity::Panorama), C(10, Amenity::Heater) };
        expect_throw([&]() { line.attach(add); }, "attach() throws if maxCabins exceeded");

        cout << "After attach fail:  " << line << endl;

        // Prüfe, dass es wirklich unverändert blieb: detach(0) sollte 2 Kabinen liefern, nicht 4
        auto detached = line.detach(0);
        check(detached.size() == 2, "attach() failure leaves state unchanged (size check)");
    }

    // --------- detach(): valid + invalid ----------
    {
        auto line = make_operational_line();
        // detach from middle
        auto part = line.detach(3);
        check(part.size() == 4, "detach(from) returns correct count");
        check(!part.empty() && part[0].get_mass() == 10, "detach(from) keeps original order (first detached mass)");

        expect_throw([&]() { line.detach(100); }, "detach(invalid index) throws");
    }
/*
    // --------- group_by_amenity(): stable grouping ----------
    {
        int drive = 60, maxLoad = 500;
        size_t maxCabins = 50;

        // Absichtlich gemischt, mit doppelten Amenities, um Stabilität zu testen
        vector<Cabin> cabins = {
            C(5, Amenity::Standard),   // Standard #1
            C(6, Amenity::Heater),     // Heater #1
            C(7, Amenity::Standard),   // Standard #2
            C(8, Amenity::Heater),     // Heater #2
            C(9, Amenity::FirstAid),   // FirstAid #1
            C(10, Amenity::BikeRack),  // BikeRack #1
            C(11, Amenity::Panorama),  // Panorama #1
            C(12, Amenity::FirstAid)   // FirstAid #2
        };

        CableCarLine line(drive, maxLoad, maxCabins, cabins);
        line.group_by_amenity();

        // Nach enum: Heater, Panorama, BikeRack, Standard, FirstAid
        // Stabilitätscheck: Heater #1 (mass 6) muss vor Heater #2 (mass 8) bleiben
        // Standard #1 (5) vor Standard #2 (7), FirstAid #1 (9) vor FirstAid #2 (12)
        auto all = line.detach(0);

        // Finde die Reihenfolge über Massen (eindeutig hier)
        auto idx = [&](int m)->int {
            for (size_t i = 0; i < all.size(); ++i) if (all[i].get_mass() == m) return (int)i;
            return -1;
        };

        check(idx(6) != -1 && idx(8) != -1 && idx(6) < idx(8), "group_by_amenity() stable within Heater");
        check(idx(5) != -1 && idx(7) != -1 && idx(5) < idx(7), "group_by_amenity() stable within Standard");
        check(idx(9) != -1 && idx(12) != -1 && idx(9) < idx(12), "group_by_amenity() stable within FirstAid");
    }

    // --------- reroute(): valid move + exception safety ----------
    {
        auto a = make_operational_line();

        // Ziel-Linie mit Platz
        int drive = 70, maxLoad = 700;
        size_t maxCabins = 30;
        vector<Cabin> bCabins = {
            C(7, Amenity::Standard),
            C(8, Amenity::FirstAid),
            C(9, Amenity::Heater),
            C(10, Amenity::Panorama),
            C(11, Amenity::BikeRack)
        };
        CableCarLine b(drive, maxLoad, maxCabins, bCabins);

        // Move 2 cabins from a (from=1,count=2) into b at pos=2
        // a: [Std(7), FirstAid(8), Panorama(9), FirstAid(10), BikeRack(11), FirstAid(12), Heater(13)]
        // moved: FirstAid(8), Panorama(9)
        a.reroute(1, 2, b, 2);

        cout << "After reroute A: " << a << endl;
        cout << "After reroute B: " << b << endl;

        // Grobe Checks via detach sizes
        auto aAll = a.detach(0);
        auto bAll = b.detach(0);

        check(aAll.size() == 5, "reroute() removes correct count from source");
        check(bAll.size() == 7, "reroute() inserts correct count into target");

        // Exception safety: pos invalid
        auto a2 = make_operational_line();
        CableCarLine b2(drive, maxLoad, maxCabins, bCabins);

        expect_throw([&]() { a2.reroute(1, 2, b2, 999); }, "reroute() throws on invalid pos");

        // State unchanged after failed reroute: check sizes remain original
        auto a2All = a2.detach(0);
        auto b2All = b2.detach(0);
        check(a2All.size() == 7, "reroute() failure keeps source unchanged");
        check(b2All.size() == 5, "reroute() failure keeps target unchanged");
    }
*/
    cout << "\n==== SUMMARY ====\n";
    cout << "Tests:  " << g_tests << "\n";
    cout << "Failed: " << g_failed << "\n";
    cout << (g_failed == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");
    return (g_failed == 0) ? 0 : 1;
}