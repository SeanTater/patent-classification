#include "pftapsparser.h"

PFTAPSParser::PFTAPSParser()
{
    // starts with HHHHHT
}

vector<Patent> PFTAPSParser::parseText(string filename)
{
    // Make an exemplary tree
    unordered_set<string> rules;
    rules.insert("ROOT PATN");

    rules.insert("PATN ASBT");
    rules.insert("ABST STM ");
    rules.insert("ABST NUM ");
    rules.insert("ABST PAC ");
    rules.insert("ABST PAR ");
    rules.insert("ABST PAL ");
    rules.insert("ABST PA1 ");

    rules.insert("PATN ASSG");

    rules.insert("PATN BSUM");
    rules.insert("BSUM STM ");
    rules.insert("BSUM NUM ");
    rules.insert("BSUM PAC ");
    rules.insert("BSUM PAR ");
    rules.insert("BSUM PA1 ");

    rules.insert("PATN CLAS");

    rules.insert("PATN CLMS");
    rules.insert("CLMS STM ");
    rules.insert("CLMS NUM ");
    rules.insert("CLMS PAC ");
    rules.insert("CLMS PAR ");
    rules.insert("CLMS PA1 ");

    rules.insert("PATN DETD`");
    rules.insert("DETD STM ");
    rules.insert("DETD NUM ");
    rules.insert("DETD PAC ");
    rules.insert("DETD PAR ");
    rules.insert("DETD PA1 ");

    rules.insert("PATN DRWD");
    rules.insert("DRWD STM ");
    rules.insert("DRWD NUM ");
    rules.insert("DRWD PAC ");
    rules.insert("DRWD PAR ");
    rules.insert("DRWD PA1 ");

    rules.insert("PATN FREF"); // ?
    rules.insert("PATN GOVT"); // ?
    rules.insert("PATN INVT");
    rules.insert("PATN LREP"); // ?
    rules.insert("PATN OREF"); // ?
    rules.insert("PATN PARN"); // ?
    rules.insert("PATN PRIR"); // ?
    rules.insert("PATN REIS"); // ?
    rules.insert("PATN RLAP"); // ?
    rules.insert("PATN UREF"); // ?

    // The document and it's type heirarchy share implementation
    // <tag_type, document_tag>
    PFTAPSTag root {"ROOT"};

    // Read the whole file line by line
    vector<string> lines;
    ifstream source(filename);
    string line;
    for (getline(source, line); not line.empty(); getline(source, line)){
        lines.push_back(line);
    }
    //reverse(lines.begin(), lines.end());
    uint furthest_read = parseLines(root, lines, 1, rules);

    cout << "Found " << root.children.size() << " patents by line " << furthest_read << endl;

    // wrong but keeps the compiler from complaining
    return vector<Patent>();

}

unsigned int PFTAPSParser::parseLines(PFTAPSTag& parent,
                    vector<string>& lines,
                    unsigned int line_i,
                    unordered_set<string>& rules) {

    // For each line starting from line_i
    while (line_i < lines.size()) {
        string name = lines[line_i].substr(0, 4);
        string value = lines[line_i].substr(4);
        boost::trim(value);

        if (name == "    ") {
            // concatenate
            parent.value += " " + value;
            line_i++;
        } else if (rules.find(parent.name + " " + name) != rules.end()) {
            // valid child
            line_i = parseLines(parent.put(name, value), lines, line_i + 1, rules);
        } else {
            // Not a valid child. Stop parsing for this node.
            break;
        }
    }
    return line_i;
}
