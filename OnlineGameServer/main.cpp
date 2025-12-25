// main.cpp
// Tests für "Online-Game Server – Player & GameServer"
//
// Erwartete Schnittstellen:
//
// enum class Effect { Poison, Shield, Speed, Invisibility, Regeneration };
//
// class Player {
// public:
//   Player(int id, int hp = 100);
//   int get_id() const;
//   int get_hp() const;
//   bool has_effect(Effect e) const;
//   void add_effect(Effect e, int duration);
//   void tick(); // -1 duration, remove 0, poison -> hp -= 5, hp not below 0
//   // operator<< optional
// };
//
// class GameServer {
// public:
//   void add_player(const Player&);
//   void remove_player(int id);
//   void apply_round(); // tick all, remove players with hp == 0
//   std::unordered_map<Effect,int> effect_statistics() const;
//   void cleanse(Effect e);              // Zusatz 10P
//   Player& most_affected_player();      // Zusatz 15P
//   // operator<< optional
// };
//
// WICHTIG: Passe den Include an deinen Dateinamen an.

#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::unordered_map;
using std::runtime_error;

// >>> ANPASSEN <<<
#include "gameServer.h"

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

// Helper: safely read stats value (missing => 0)
int get_stat(const unordered_map<Effect,int>& m, Effect e) {
    auto it = m.find(e);
    return (it == m.end()) ? 0 : it->second;
}

int main() {
    cout << "==== PR1 Probeklausur Tests: Online-Game Server ====\n" << endl;

    // ---------------- Player ctor validation ----------------
    expect_throw([](){ Player p(0, 100); }, "Player id must be > 0");
    expect_throw([](){ Player p(1, 0); }, "Player hp must be in [1..100] (low)");
    expect_throw([](){ Player p(1, 101); }, "Player hp must be in [1..100] (high)");

    // ---------------- add_effect validation ----------------
    {
        Player p(1, 100);
        expect_throw([&](){ p.add_effect(Effect::Poison, 0); }, "add_effect duration <= 0 throws");
        expect_throw([&](){ p.add_effect(Effect::Poison, -3); }, "add_effect duration < 0 throws");
        check(!p.has_effect(Effect::Poison), "has_effect false initially");
        p.add_effect(Effect::Poison, 2);
        check(p.has_effect(Effect::Poison), "has_effect true after add_effect");
    }

    // ---------------- tick(): duration decreases and expires ----------------
    {
        Player p(2, 100);
        p.add_effect(Effect::Speed, 1);
        check(p.has_effect(Effect::Speed), "Speed present before tick");
        p.tick(); // duration 1->0, should be removed
        check(!p.has_effect(Effect::Speed), "Speed removed when duration reaches 0");
    }

    // ---------------- tick(): poison damage and clamp at 0 ----------------
    {
        Player p(3, 7);
        p.add_effect(Effect::Poison, 3);

        p.tick(); // hp 7->2
        check(p.get_hp() == 2, "Poison tick reduces hp by 5");

        p.tick(); // hp 2->0 (clamp)
        check(p.get_hp() == 0, "HP clamped at 0 (no negative)");

        // poison might still exist for a tick depending on your order:
        // spec says: durations -1, remove 0, poison -> hp -= 5
        // This test doesn't require poison presence after death.
    }

    // ---------------- add_effect(): replace duration if already present ----------------
    {
        Player p(4, 100);
        p.add_effect(Effect::Shield, 1);
        p.add_effect(Effect::Shield, 5); // replace, not stack
        // We cannot directly read duration, but we can infer by ticking:
        for (int i = 0; i < 4; ++i) p.tick();
        check(p.has_effect(Effect::Shield), "Shield still present after 4 ticks if replaced to 5");
        p.tick();
        check(!p.has_effect(Effect::Shield), "Shield removed after total 5 ticks");
    }

    // ---------------- GameServer: add/remove player ----------------
    {
        GameServer s;
        Player a(10, 100);
        Player b(11, 100);

        s.add_player(a);
        s.add_player(b);
        expect_throw([&](){ s.add_player(a); }, "add_player duplicate id throws");

        s.remove_player(11);
        expect_throw([&](){ s.remove_player(11); }, "remove_player unknown id throws");
    }

    // ---------------- apply_round(): ticks all and removes dead players ----------------
    {
        GameServer s;
        Player p1(1, 100);
        Player p2(2, 5); // will die after poison tick
        Player p3(3, 100);

        p1.add_effect(Effect::Speed, 2);
        p2.add_effect(Effect::Poison, 1);
        p3.add_effect(Effect::Shield, 3);

        s.add_player(p1);
        s.add_player(p2);
        s.add_player(p3);

        // Round 1: p2 hp 5->0 -> removed
        s.apply_round();

        auto stats = s.effect_statistics();
        // remaining players: p1 has Speed(1), p3 has Shield(2)
        check(get_stat(stats, Effect::Speed) == 1, "stats Speed count after round");
        check(get_stat(stats, Effect::Shield) == 1, "stats Shield count after round");
        check(get_stat(stats, Effect::Poison) == 0, "stats Poison count after dead player removed");
    }

    // ---------------- effect_statistics(): multiple effects per player ----------------
    {
        GameServer s;
        Player a(20, 100), b(21, 100), c(22, 100);

        a.add_effect(Effect::Poison, 2);
        a.add_effect(Effect::Speed,  2);

        b.add_effect(Effect::Poison, 1);

        c.add_effect(Effect::Shield, 3);
        c.add_effect(Effect::Speed,  1);

        s.add_player(a);
        s.add_player(b);
        s.add_player(c);

        auto stats = s.effect_statistics();
        check(get_stat(stats, Effect::Poison) == 2, "stats: Poison on 2 players");
        check(get_stat(stats, Effect::Speed)  == 2, "stats: Speed on 2 players");
        check(get_stat(stats, Effect::Shield) == 1, "stats: Shield on 1 player");
        check(get_stat(stats, Effect::Invisibility) == 0, "stats: missing effects may be absent => treated as 0");
    }

    // ---------------- cleanse(): remove effect from all, throw if none had it ----------------
    {
        GameServer s;
        Player a(30, 100), b(31, 100), c(32, 100);

        a.add_effect(Effect::Poison, 2);
        b.add_effect(Effect::Poison, 1);
        c.add_effect(Effect::Shield, 3);

        s.add_player(a);
        s.add_player(b);
        s.add_player(c);

        s.cleanse(Effect::Poison);

        auto stats = s.effect_statistics();
        check(get_stat(stats, Effect::Poison) == 0, "cleanse removes Poison from all players");

        expect_throw([&](){ s.cleanse(Effect::Invisibility); }, "cleanse throws if no player had that effect");
    }

    // ---------------- most_affected_player(): max effects, tie -> lowest hp ----------------
    {
        GameServer s;
        Player a(40, 90), b(41, 50), c(42, 50);

        // a has 1 effect
        a.add_effect(Effect::Shield, 3);

        // b has 2 effects, hp=50
        b.add_effect(Effect::Poison, 2);
        b.add_effect(Effect::Speed,  2);

        // c has 2 effects, hp=50, but we want a deterministic tie breaker:
        // let's make c hp=60 so b wins by lower hp.
        c = Player(42, 60);
        c.add_effect(Effect::Poison, 2);
        c.add_effect(Effect::Speed,  2);

        s.add_player(a);
        s.add_player(b);
        s.add_player(c);

        Player& best = s.most_affected_player();
        check(best.get_id() == 41, "most_affected_player picks player with most effects; tie -> lowest hp");
    }

    // ---------------- most_affected_player(): throws if server empty ----------------
    {
        GameServer s;
        expect_throw([&](){ (void)s.most_affected_player(); }, "most_affected_player throws if no players");
    }

    cout << "\n==== SUMMARY ====\n";
    cout << "Tests:  " << g_tests << "\n";
    cout << "Failed: " << g_failed << "\n";
    cout << (g_failed == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");

    return (g_failed == 0) ? 0 : 1;
}





