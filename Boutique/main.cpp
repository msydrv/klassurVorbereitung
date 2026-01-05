// main.cpp
// Tests für Aufgabe "Boutique" (Garment + Boutique)
//
// Erwartete Schnittstellen (aus Angabe):
//
// enum class Designer { Prada, Chanel, Dior, Unknown };
//
// class Garment {
// public:
//   Garment(std::string description, int stock, Designer d = Designer::Unknown);
//   bool by_designer(const std::vector<Designer>& vd) const;
//   int available() const;                         // stock - reserved.size()
//   bool reserve(const std::string& name);         // throws if name empty
//   bool buy(const std::string& name);             // Zusatz 10P
//   // operator<<: [description: designer, available, {reservations}]
// };
//
// class Boutique {
// public:
//   Boutique(std::vector<Garment> offer);          // throws if offer empty
//   std::vector<Garment> selection(const std::vector<Designer>& vd) const;
//   std::vector<Garment> sale(Designer d);         // Zusatz 15P
//   // operator<<: [{list of garments}] with *...* for not available
// };
//
// WICHTIG: Passe den Include unten an deinen Header an.

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::runtime_error;

// >>> ANPASSEN <<<
#include "boutique.h"

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

// Helper: pretty print a vector<Garment> (relies on operator<< Garment)
void print_list(const vector<Garment>& v, const string& title) {
    cout << title << " (" << v.size() << "): ";
    cout << "{ ";
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i];
        if (i + 1 < v.size()) cout << ", ";
    }
    cout << " }" << endl;
}

int main() {
    cout << "==== PR1 Tests: Boutique (Garment + Boutique) ====\n" << endl;

    // ---------------- Garment ctor validation ----------------
    expect_throw([](){ Garment g("", 3, Designer::Dior); }, "Garment ctor throws if description empty");
    expect_throw([](){ Garment g("Skirt", -1, Designer::Prada); }, "Garment ctor throws if stock negative");

    // ---------------- Garment basics: by_designer / available / reserve ----------------
    {
        Garment shoe("Exquisite Shoe", 4, Designer::Dior);
        check(shoe.available() == 4, "available() initially equals stock");

        vector<Designer> d1 = {Designer::Dior};
        vector<Designer> d2 = {Designer::Prada, Designer::Chanel};
        check(shoe.by_designer(d1), "by_designer true for matching designer");
        check(!shoe.by_designer(d2), "by_designer false for non-matching list");

        expect_throw([&](){ (void)shoe.reserve(""); }, "reserve throws if name empty");

        // reserve ok
        check(shoe.reserve("Herbert"), "reserve returns true when possible");
        check(shoe.available() == 3, "available decreases after reserve");

        // duplicate reserve
        check(!shoe.reserve("Herbert"), "reserve returns false for duplicate name");

        // fill remaining availability
        check(shoe.reserve("Susanne"), "reserve 2nd");
        check(shoe.reserve("Maria"), "reserve 3rd");
        check(shoe.reserve("Ali"), "reserve 4th");
        check(shoe.available() == 0, "available becomes 0 when reserved == stock");

        // no more availability
        check(!shoe.reserve("Nina"), "reserve returns false if none available");

        cout << "Garment print example: " << shoe << endl;
    }

    // ---------------- Boutique ctor validation ----------------
    expect_throw([](){ Boutique b(vector<Garment>{}); }, "Boutique ctor throws if offer empty");

    // Build offer list
    Garment g1("Exquisite Shoe", 4, Designer::Dior);
    Garment g2("Skirt", 1, Designer::Prada);
    Garment g3("T-Shirt", 10, Designer::Unknown);
    Garment g4("Jacket", 2, Designer::Chanel);
    Garment g5("Dress", 2, Designer::Prada);

    // Make some not available by reserving up to stock
    (void)g2.reserve("Maria");          // stock 1 -> available 0
    (void)g1.reserve("Herbert");
    (void)g1.reserve("Susanne");        // still available 2

    vector<Garment> offer = {g1, g2, g3, g4, g5};
    Boutique shop(offer);

    cout << "Boutique print example:\n" << shop << endl;

    // ---------------- Boutique::selection ----------------
    {
        vector<Designer> pick = {Designer::Prada, Designer::Dior};
        auto sel = shop.selection(pick);

        // selection: designer in pick AND at least one available
        // g1 Dior available 2 -> included
        // g2 Prada available 0 -> excluded
        // g5 Prada available 2 -> included
        check(sel.size() == 2, "selection returns only matching designer with available > 0");

        // relative order must match offer list order: g1 appears before g5
        // We can't directly compare by description unless you have getters;
        // so we print and do a weak check: first element should be Dior item (g1).
        // If your Garment<< prints designer name, this is easy to eyeball.
        print_list(sel, "selection(Dior,Prada)");
    }

    // -------------- Zusatz 10P: Garment::buy ----------------
    {
        Garment x("Limited Bag", 2, Designer::Chanel);

        // buy without reservation should reduce stock if available
        check(x.buy("Anna"), "buy succeeds with stock available (no reservation needed)");
        check(x.available() == 1, "buy reduces stock by 1 (available reflects it)");

        // reserve then buy should remove reservation and reduce stock
        check(x.reserve("Bob"), "reserve Bob");
        int beforeAvail = x.available();
        check(x.buy("Bob"), "buy with existing reservation succeeds");
        check(x.available() == beforeAvail, "buy with reservation: reservation removed and stock-- => available unchanged");

        // no availability left -> buy false
        // At this point: initial stock 2, bought Anna => stock 1, reserved Bob then bought Bob => stock 0
        check(!x.buy("Chris"), "buy returns false if no stock available and no reservation");
    }

    // ---------------- Zusatz 15P: Boutique::sale ----------------
    {
        // We want to remove all Prada garments that have NO reservations.
        // And return all Prada garments that could NOT be removed (because they have reservations).

        // Create a fresh boutique to avoid side-effects from earlier.
        Garment p1("Prada Coat", 2, Designer::Prada);       // no reservation -> should be removed
        Garment p2("Prada Skirt", 1, Designer::Prada);      // reservation -> should stay, returned
        Garment d1("Dior Shoe", 3, Designer::Dior);         // other designer -> stays

        (void)p2.reserve("Maria");

        Boutique b2(vector<Garment>{p1, p2, d1});

        auto blocked = b2.sale(Designer::Prada);

        // blocked should contain p2 only
        check(blocked.size() == 1, "sale returns list of garments that could not be removed due to reservations");

        cout << "Blocked (Prada) from sale:\n";
        print_list(blocked, "blocked");

        cout << "Boutique after sale(Prada):\n" << b2 << endl;

        // Note: Without getters, we cannot perfectly assert which one remained.
        // But visually, you should see:
        // - Prada Coat removed
        // - Prada Skirt still present (probably marked *...* if not available)
        // - Dior Shoe still present
    }

    cout << "\n==== SUMMARY ====\n";
    cout << "Tests:  " << g_tests << "\n";
    cout << "Failed: " << g_failed << "\n";
    cout << (g_failed == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");

    return (g_failed == 0) ? 0 : 1;
}
