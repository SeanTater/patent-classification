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

public:
    string name;
    string value;
    vector<PFTAPSTag> children;

    PFTAPSTag(string name, string value="") {
        this->name = name;
        this->value = value;
    }

    PFTAPSTag& put(const string& name, const string& value) {
        children.emplace_back(name, value);
        return children.back();
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
private:
    static unsigned int parseLines(PFTAPSTag& parent,
                        vector<string>& lines,
                        unsigned int line_i,
                        unordered_set<string>& rules);
public:
    PFTAPSParser();
    static vector<Patent> parseText(string filename);
};

#endif // PFTAPSPARSER_H
