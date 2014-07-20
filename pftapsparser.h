#ifndef PFTAPSPARSER_H
#define PFTAPSPARSER_H
#include <patent.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <unordered_set>
#include <string>
#include <algorithm>

using namespace std;

class PFTAPSTag {
    friend class PFTAPSParser;
private:
    string name;
    string value;
    vector<PFTAPSTag> children;

public:
    PFTAPSTag(const string &name, const vector<PFTAPSTag>& children={} ) {
        this->name = name;
        this->value = value;
        this->children = children;
    }

    PFTAPSTag put(const string& name, const string& value) {
        PFTAPSTag t = PFTAPSTag(name);
        t.value = value;
        return t;
    }

    PFTAPSTag child(const string & name) {
        for (auto tag : children)
            if (tag.name == name)
                return tag;
        return PFTAPSTag("");
    }

    bool empty() {
        return name.empty();
    }
};


class PFTAPSParser : public Patent
{
public:
    PFTAPSParser();
    static vector<Patent> parseText(string filename);
};

#endif // PFTAPSPARSER_H
