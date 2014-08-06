#include "pftapsparser.h"

PFTAPSParser::PFTAPSParser()
{
}

/**
 * @brief Concatenate paragraphs under a node
 * @param parent
 * @return
 *
 * It could be useful later to separate the paragraphs so keep this in mind.
 */
string extractText(const PFTAPSTag& parent, const string& delim="\n") {
    string out;
    for (const PFTAPSTag& child : parent.children) {
        out += child.value + delim;
    }
    return out;
}

string extractClaims(const PFTAPSTag& parent) {
    string out;
    for (const PFTAPSTag& child : parent.children) {
        if (child.name == "NUM ") {
            out += "\x1d";
        } else {
            out += child.value + "\x1e";
        }
    }
    return out;
}

vector<Patent> PFTAPSParser::parseText(string filename) {
    // Get a document (multiple patents)
    PFTAPSTag doc = asTree(filename);

    vector<Patent> patents;

    doc.eachName("PATN", [&patents](const PFTAPSTag& pat_node){
        Patent pat;
        pat.setAbstract(extractText(pat_node.child("ABST")));
        pat.setClaims(extractClaims(pat_node.child("CLMS")));
        pat.setDescription(extractText(pat_node.child("BSUM")));
        pat.setTitle(pat_node.child("TTL ").value);
        /* This is the real patent number:
         *      pat.setId(pat_node.child("REIS").child("PNO ").value);
         * in some, it is missing! */
        pat.setId(pat_node.child("WKU ").value);
        // classifications now
        for (const PFTAPSTag& klass : pat_node.child("CLAS").children) {
            if (klass.name == "ICL ") {
                pat.appendClass(pat.ipc, klass.value);
            } else if ((klass.name == "OCL " or klass.name == "XCL ") and not klass.value.empty()) {
                string value = klass.value.substr(0, 3);
                boost::trim(value);
                if (klass.value.length() > 3) {
                    string subsection = klass.value.substr(3, 3);
                    boost::trim(subsection);
                    value += "/" + subsection;
                }
                if (klass.value.length() > 6) {
                    string subsubsection = klass.value.substr(6, 2);
                    boost::trim(subsubsection);
                    value += "." + subsubsection;
                }
                pat.appendClass(pat.uspc, value);
            }
        }
        patents.push_back(pat);
    });
    return patents;
}

PFTAPSTag PFTAPSParser::asTree(string filename)
{
    /* Rules: the right can be a child of the left
     * These are only the block-style nodes because there are too many line
     * style nodes.
     */
    unordered_set<string> rules;
    rules.insert("ROOT PATN");

    rules.insert("PATN ABST");
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
    vector<string> stack {"ROOT"};
    unsigned int furthest_read = parseLines(root, stack, lines, 1, rules);

    cout << "Found " << root.children.size() << " patents by line " << furthest_read << endl;

    return root;

}

// Convenience method for parseLines
bool childOfAny(const string& name, const vector<string>& ancestors, const unordered_set<string>& rules) {
    for (const string& ancestor : ancestors)
        if (rules.find(ancestor + " " + name) != rules.end())
            return true;
    return false;
}


unsigned int PFTAPSParser::parseLines(PFTAPSTag& parent,
                    vector<string>& ancestors,
                    vector<string>& lines,
                    unsigned int line_i,
                    unordered_set<string>& rules) {

    // For each line starting from line_i
    while (line_i < lines.size()) {
        string name = lines[line_i].substr(0, 4);
        string value;
        if (lines[line_i].length() > 5)
            value = lines[line_i].substr(5); // There is an intervening space
        boost::trim(value);

        if (name == "    ") {
            // concatenate
            parent.value += " " + value;
            line_i++;
        } else if (rules.find(parent.name + " " + name) != rules.end()) {
            // valid block-type child
            ancestors.push_back(name);
            line_i = parseLines(parent.put(name, value), ancestors, lines, line_i + 1, rules);
            ancestors.pop_back();
        } else if (childOfAny(name, ancestors, rules)) {
            // Stop parsing this child.
            break;
        } else {
            // line-type child
            parent.put(name, value);
            line_i++;
        }
    }
    return line_i;
}
