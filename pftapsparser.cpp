#include "pftapsparser.h"

PFTAPSParser::PFTAPSParser()
{
    // starts with HHHHHT
}

vector<Patent> PFTAPSParser::parseText(string filename)
{
    // Make an exemplary tree
    unordered_set<string> heirarchy;
    heirarchy.insert("ROOT PATN");

    heirarchy.insert("PATN ASBT");
    heirarchy.insert("ABST STM ");
    heirarchy.insert("ABST NUM ");
    heirarchy.insert("ABST PAC ");
    heirarchy.insert("ABST PAR ");
    heirarchy.insert("ABST PAL ");
    heirarchy.insert("ABST PA1 ");

    heirarchy.insert("PATN ASSG");

    heirarchy.insert("PATN BSUM");
    heirarchy.insert("BSUM STM ");
    heirarchy.insert("BSUM NUM ");
    heirarchy.insert("BSUM PAC ");
    heirarchy.insert("BSUM PAR ");
    heirarchy.insert("BSUM PA1 ");

    heirarchy.insert("PATN CLAS");

    heirarchy.insert("PATN CLMS");
    heirarchy.insert("CLMS STM ");
    heirarchy.insert("CLMS NUM ");
    heirarchy.insert("CLMS PAC ");
    heirarchy.insert("CLMS PAR ");
    heirarchy.insert("CLMS PA1 ");

    heirarchy.insert("PATN DETD`");
    heirarchy.insert("DETD STM ");
    heirarchy.insert("DETD NUM ");
    heirarchy.insert("DETD PAC ");
    heirarchy.insert("DETD PAR ");
    heirarchy.insert("DETD PA1 ");

    heirarchy.insert("PATN DRWD");
    heirarchy.insert("DRWD STM ");
    heirarchy.insert("DRWD NUM ");
    heirarchy.insert("DRWD PAC ");
    heirarchy.insert("DRWD PAR ");
    heirarchy.insert("DRWD PA1 ");

    heirarchy.insert("PATN FREF"); // ?
    heirarchy.insert("PATN GOVT"); // ?
    heirarchy.insert("PATN INVT");
    heirarchy.insert("PATN LREP"); // ?
    heirarchy.insert("PATN OREF"); // ?
    heirarchy.insert("PATN PARN"); // ?
    heirarchy.insert("PATN PRIR"); // ?
    heirarchy.insert("PATN REIS"); // ?
    heirarchy.insert("PATN RLAP"); // ?
    heirarchy.insert("PATN UREF"); // ?

    // The document and it's type heirarchy share implementation
    // <tag_type, document_tag>
    vector<pair<PFTAPSTag, PFTAPSTag> > parse_stack { pair(PFTAPSTag("ROOT"), PFTAPSTag("ROOT")) };

    // Read the whole file line by line
    vector<string> lines;
    ifstream source(filename);
    string line;
    for (getline(source, line); not line.empty(); getline(source, line)){
        lines.push_back(line);
    }
    //reverse(lines.begin(), lines.end());

    for (string& line : lines) {
        PFTAPSTag& doc_top = parse_stack.back().second();

        string name = line.substr(0, 4);
        string value = line.substr(4);
        boost::trim(value);

        // "    " means concatenate
        if (name == "    ") {
            doc_top.value += " " + value;
        } else {
            // Three choices:
            //   It's a child tag: put it in children
            //   else
            //     If it is ROOT, discard the node
            //     else:
            //       pop the stack and move one tag up. Repeat.)
            pair<PFTAPSTag, PFTAPSTag>& parse_top = parse_stack.back();
            auto next_type = parse_top.first.child(name);
            while (next_type.empty() && name != "ROOT") {
                parse_top.pop_back();

            }
            if (not next_type.empty()) {
                parse_top.emplace_back(next_type, parse_top.second.put(name, value));
            } else {
                while (name != "ROOT") {
                    parse_top.pop_back();

                }
            }
        }

        top.put(name, value);
    }

    // wrong but keeps the compiler from complaining
    return vector<Patent>();

}

void parseLines(PFTAPSTag parent,
                    vector<string> lines,
                    unsigned int line_i,
                    unordered_set<string> rules) {


    if (line_i == lines.length()) {
        return;
    }
    string name = lines[line_i].substr(0, 4);
    string value = lines[line_i].substr(4);
    boost::trim(value);


    while (name == "    " // concatenate
           or heirarchy.find(parent.name + " " + name) != heirarchy.end() // valid child
           ) {
        if (name == "    ") {
            // concatenate
            parent.value += " " + value;
        } else {
            // valid child
            parseLines(parent.put(name, value), lines, line_i + 1, rules);
        }
    }
    // Not a valid child. Stop parsing for this node.
}


