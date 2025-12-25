// main.cpp
// Tests für die Probeklausur "MuseumSecurity – Artifact & Patrol"
//
// Erwartete Schnittstellen (wie in der Angabe):
// enum class Tag { Fragile, Painting, Sculpture, Digital, Relic };
// class Artifact { Artifact(int level, Tag t = Tag::Relic); int get_level() const; bool has_tag(Tag) const; };
// class Patrol {
//   Patrol(int fatigue_limit, int risk_cap, std::vector<Artifact> list = {});
//   int total_risk() const;
//   bool approved() const;
//   void add(std::vector<Artifact>);
//   std::vector<Artifact> cut(size_t from);
//   void stable_group();                       // Zusatz 10P
//   void splice(size_t from, size_t count, Patrol& to, size_t pos); // Zusatz 15P
// };
// operator<< für Artifact und Patrol existiert.
//
// WICHTIG: Passe den Include unten an deinen Dateinamen an.

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::runtime_error;
using std::string;

// >>> ANPASSEN <<<
#include "patrol.h"

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

// ---------------- Helpers ----------------
Artifact A(int lvl, Tag t) { return Artifact(lvl, t); }

int sum_levels(const vector<Artifact>& v) {
    int s = 0;
    for (const auto& a : v) s += a.get_level();
    return s;
}

// Eine genehmigte Route bauen:
// - alle Tags mind. 1x
// - nie 2 Relic hintereinander
// - Fragile-Regel (Nachbarn sum <= 10)
// - Digital -> später Painting
// - risk unter cap (wir wählen großzügig)
Patrol make_approved_patrol() {
    int fatigue_limit = 60;
    int risk_cap = 2000;

    // Reihenfolge bewusst gewählt:
    // 0: Relic(3)
    // 1: Digital(4)      -> später Painting kommt bei 3
    // 2: Sculpture(3)
    // 3: Painting(2)
    // 4: Fragile(5)      -> Nachbarn: Painting(2)+Relic(3)=5 <= 10
    // 5: Relic(3)        -> nie zwei Relic hintereinander, ok
    vector<Artifact> list = {
        A(3, Tag::Relic),
        A(4, Tag::Digital),
        A(3, Tag::Sculpture),
        A(2, Tag::Painting),
        A(5, Tag::Fragile),
        A(3, Tag::Relic)
    };

    return Patrol(fatigue_limit, risk_cap, list);
}

int main() {
    cout << "==== PR1 Probeklausur Tests: MuseumSecurity ====\n" << endl;

    // ---------------- Artifact: Konstruktor/Validierung ----------------
    expect_throw([](){ Artifact x(0); }, "Artifact level < 1 throws");
    expect_throw([](){ Artifact x(10, Tag::Painting); }, "Artifact level > 9 throws");

    {
        Artifact d(7); // default Tag::Relic
        check(d.get_level() == 7, "Artifact get_level()");
        check(d.has_tag(Tag::Relic), "Artifact default tag is Relic");
        check(!d.has_tag(Tag::Digital), "Artifact has_tag false case");
        cout << "Artifact print: " << d << endl;
    }

    // ---------------- Patrol: Konstruktor/Validierung ----------------
    expect_throw([](){ Patrol p(9, 100); }, "Patrol fatigue_limit < 10 throws");
    expect_throw([](){ Patrol p(10, 14); }, "Patrol risk_cap < 15 throws");

    {
        // Startliste verletzt fatigue_limit -> muss throw
        vector<Artifact> too_tiring = { A(9, Tag::Relic), A(9, Tag::Painting) }; // sum = 18
        expect_throw([&](){ Patrol p(15, 500, too_tiring); }, "Patrol ctor throws if initial sumLevels > fatigue_limit");
    }

    // ---------------- total_risk ----------------
    {
        auto p = make_approved_patrol();
        // risk = 3^2 + 4^2 + 3^2 + 2^2 + 5^2 + 3^2 = 9+16+9+4+25+9 = 72
        check(p.total_risk() == 72, "Patrol total_risk() correct");
        cout << "Patrol print: " << p << endl;
    }

    // ---------------- approved(): positive case ----------------
    {
        auto p = make_approved_patrol();
        check(p.approved(), "approved() true for valid route");
    }

    // ---------------- approved(): missing tags (Regel 2) ----------------
    {
        int fatigue_limit = 60, risk_cap = 2000;
        // fehlt Sculpture und Fragile
        vector<Artifact> list = {
            A(3, Tag::Relic),
            A(4, Tag::Digital),
            A(2, Tag::Painting),
            A(3, Tag::Relic)
        };
        Patrol p(fatigue_limit, risk_cap, list);
        check(!p.approved(), "approved() false when not all tags are present");
    }

    // ---------------- approved(): Relic-Relic (Regel 3) ----------------
    {
        int fatigue_limit = 60, risk_cap = 2000;
        vector<Artifact> list = {
            A(3, Tag::Relic),
            A(2, Tag::Relic),      // two Relic in a row
            A(4, Tag::Digital),
            A(2, Tag::Painting),
            A(5, Tag::Fragile),
            A(3, Tag::Sculpture)
        };
        Patrol p(fatigue_limit, risk_cap, list); // <- Tippfehler? Nein, absichtlich NICHT.
        // WICHTIG: Lösche die nächste Zeile, wenn du saubere Tests willst.
        // Diese Zeile soll dich zwingen, auf Compilerfehler zu reagieren.
        // KORREKT wäre: Patrol p(fatigue_limit, risk_cap, list);

        // check(!p.approved(), "approved() false for Relic-Relic adjacency");
    }

    // ---------------- approved(): Fragile-Nachbarn Regel (4) ----------------
    {
        int fatigue_limit = 60, risk_cap = 2000;
        // Fragile in der Mitte, Nachbarn sum > 10 => false
        vector<Artifact> list = {
            A(3, Tag::Relic),
            A(6, Tag::Painting),
            A(5, Tag::Fragile),    // neighbors 6 + 6 = 12 > 10
            A(6, Tag::Sculpture),
            A(4, Tag::Digital)
        };
        // alle Tags fehlen hier -> wir ergänzen, ohne die fragile Nachbarn-Regel zu fixen:
        list.push_back(A(3, Tag::Relic)); // ensure Relic exists (already)
        // Es fehlt noch: (je nach list) alle Tags: Fragile, Painting, Sculpture, Digital, Relic sind da -> ok
        Patrol p(fatigue_limit, risk_cap, list);
        check(!p.approved(), "approved() false due to Fragile neighbor rule");
    }

    // ---------------- approved(): Digital aber kein Painting danach (Regel 5) ----------------
    {
        int fatigue_limit = 60, risk_cap = 2000;
        vector<Artifact> list = {
            A(3, Tag::Relic),
            A(4, Tag::Digital),    // Digital appears
            A(3, Tag::Sculpture),
            A(5, Tag::Fragile),
            A(2, Tag::Painting),   // Painting is AFTER Fragile but is it after Digital? yes, it's after. -> would PASS rule 5
            A(3, Tag::Relic)
        };
        // Um Regel 5 wirklich zu verletzen, muss Painting VOR Digital liegen oder fehlen.
        list = {
            A(2, Tag::Painting),   // Painting before
            A(4, Tag::Digital),    // Digital appears, but no Painting after
            A(3, Tag::Sculpture),
            A(5, Tag::Fragile),
            A(3, Tag::Relic),
            A(2, Tag::Relic)       // also violates relic adjacency, but rule 5 violation still tested
        };
        Patrol p(fatigue_limit, risk_cap, list);
        check(!p.approved(), "approved() false when Digital exists but no Painting after it");
    }

    // ---------------- add(): exception safety on fatigue overflow ----------------
    {
        int fatigue_limit = 15, risk_cap = 2000;
        vector<Artifact> list = { A(5, Tag::Relic), A(5, Tag::Painting) }; // sum=10
        Patrol p(fatigue_limit, risk_cap, list);

        cout << "Before add fail: " << p << endl;

        vector<Artifact> addList = { A(6, Tag::Digital) }; // would sum=16 > 15
        expect_throw([&](){ p.add(addList); }, "add() throws if fatigue_limit would be exceeded");

        cout << "After add fail:  " << p << endl;

        // Prüfen, dass Zustand unverändert blieb:
        auto cutAll = p.cut(0);
        check(cutAll.size() == 2, "add() failure leaves state unchanged (size check)");
        check(sum_levels(cutAll) == 10, "add() failure leaves state unchanged (sumLevels check)");
    }

    // ---------------- cut(): valid + invalid ----------------
    {
        auto p = make_approved_patrol();
        auto part = p.cut(2);
        check(part.size() == 4, "cut(from) returns correct suffix size");
        check(part[0].get_level() == 3, "cut(from) keeps original order (first suffix element)");

        expect_throw([&](){ p.cut(999); }, "cut(invalid index) throws");
    }

    // ---------------- stable_group(): stable grouping by Tag ----------------
    {
        int fatigue_limit = 80, risk_cap = 5000;

        // Massen/levels sind hier eindeutige IDs für Stabilitätschecks
        vector<Artifact> list = {
            A(1, Tag::Relic),       // Relic #1
            A(2, Tag::Fragile),     // Fragile #1
            A(3, Tag::Relic),       // Relic #2
            A(4, Tag::Painting),    // Painting #1
            A(5, Tag::Fragile),     // Fragile #2
            A(6, Tag::Digital),     // Digital #1
            A(7, Tag::Sculpture),   // Sculpture #1
            A(8, Tag::Painting)     // Painting #2
        };

        Patrol p(fatigue_limit, risk_cap, list);
        p.stable_group();

        auto all = p.cut(0); // alles rausnehmen, um Reihenfolge zu inspizieren

        auto idx = [&](int lvl)->int {
            for (size_t i = 0; i < all.size(); ++i)
                if (all[i].get_level() == lvl) return (int)i;
            return -1;
        };

        // Enum order: Fragile, Painting, Sculpture, Digital, Relic
        // Stabilität: Fragile #1 (2) vor Fragile #2 (5), Painting #1 (4) vor Painting #2 (8), Relic #1 (1) vor Relic #2 (3)
        check(idx(2) != -1 && idx(5) != -1 && idx(2) < idx(5), "stable_group() stable within Fragile");
        check(idx(4) != -1 && idx(8) != -1 && idx(4) < idx(8), "stable_group() stable within Painting");
        check(idx(1) != -1 && idx(3) != -1 && idx(1) < idx(3), "stable_group() stable within Relic");
    }
/*
    // ---------------- splice(): move segment + exception safety ----------------
    {
        auto a = make_approved_patrol();

        int fatigue_limit = 80, risk_cap = 5000;
        vector<Artifact> bList = {
            A(2, Tag::Relic),
            A(2, Tag::Painting),
            A(2, Tag::Sculpture),
            A(2, Tag::Digital),
            A(2, Tag::Fragile)
        };
        Patrol b(fatigue_limit, risk_cap, bList);

        // Move 2 artifacts from a (from=1,count=2) into b at pos=3
        a.splice(1, 2, b, 3);

        cout << "After splice A: " << a << endl;
        cout << "After splice B: " << b << endl;

        // Grobcheck: Größen
        auto aAll = a.cut(0);
        auto bAll = b.cut(0);
        check(aAll.size() == 4, "splice() removes correct count from source");
        check(bAll.size() == 7, "splice() inserts correct count into target");

        // Exception safety: invalid pos
        auto a2 = make_approved_patrol();
        Patrol b2(fatigue_limit, risk_cap, bList);

        expect_throw([&](){ a2.splice(1, 2, b2, 999); }, "splice() throws on invalid pos");
        auto a2All = a2.cut(0);
        auto b2All = b2.cut(0);
        check(a2All.size() == 6, "splice() failure keeps source unchanged");
        check(b2All.size() == 5, "splice() failure keeps target unchanged");
    }

    // ---------------- splice(): self-splice edge case ----------------
    {
        // Selbst-Splice muss korrekt funktionieren (oder sauber throwen, je nach Spezifikation).
        int fatigue_limit = 80, risk_cap = 5000;
        vector<Artifact> list = {
            A(2, Tag::Fragile),
            A(3, Tag::Painting),
            A(4, Tag::Sculpture),
            A(5, Tag::Digital),
            A(6, Tag::Relic)
        };
        Patrol p(fatigue_limit, risk_cap, list);

        // Move middle segment [1,3) to position 4 (near end)
        // expected new order:
        // original: 2F, 3P, 4S, 5D, 6R
        // cut (3P,4S) and insert so first inserted gets index 4 (before 6R):
        // -> 2F, 5D, 3P, 4S, 6R
        p.splice(1, 2, p, 4);

        auto all = p.cut(0);
        check(all.size() == 5, "self-splice preserves size");
        check(all[0].get_level() == 2, "self-splice order[0] correct");
        check(all[1].get_level() == 5, "self-splice order[1] correct");
        check(all[2].get_level() == 3, "self-splice moved segment kept order (first)");
        check(all[3].get_level() == 4, "self-splice moved segment kept order (second)");
        check(all[4].get_level() == 6, "self-splice tail correct");
    }
*/
    cout << "\n==== SUMMARY ====\n";
    cout << "Tests:  " << g_tests << "\n";
    cout << "Failed: " << g_failed << "\n";
    cout << (g_failed == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");

    return (g_failed == 0) ? 0 : 1;
}