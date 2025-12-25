#ifndef PATROL_H
#define PATROL_H

#include"artifact.h"

#include<iostream>
#include<vector>
#include<string>

class Patrol {
    int fartigue_limit;
    int risk_cap;
    std::vector<Artifact> list;
public:
    Patrol(int, int);
    Patrol(int, int, std::vector<Artifact>);

    int total_risk() const;

    bool approved() const;

    void add(std::vector<Artifact>);

    std::vector<Artifact> cut(size_t from);

    friend std::ostream& operator<<(std::ostream& o, Patrol x);

    void stable_group();

    void splice(size_t from, size_t count, Patrol& to, size_t pos);
};

std::ostream& operator<<(std::ostream& o, Patrol x);

#endif