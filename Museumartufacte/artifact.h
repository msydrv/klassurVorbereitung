#ifndef ARTIFACT_H
#define ARTIFACT_H


#include<iostream>
#include<string>
#include<vector>


enum class Tag { Fragile, Painting, Sculpture, Digital, Relic };
inline const std::vector<std::string> tag_names {"Fragile", "Painting", "Sculpture", "Digital", "Relic"};


class Artifact {
    int level;
    Tag tag;
public:
    Artifact(int);
    Artifact(int, Tag);

    int get_level() const;

    bool has_tag(Tag) const;


    friend std::ostream& operator<<(std::ostream& o, Artifact x);
};


std::ostream& operator<<(std::ostream& o, Artifact x);

#endif