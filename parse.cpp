#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <string>
#include "cstring"
#include <regex>
#include <vector>

using namespace std;
using namespace sqlite3x;

class Patent {
private:
    pugi::xml_document doc;
    pugi::xml_node root;
    bool success;
    
    /**
     * Try to extract a snippet of at least target_length characters from the
     * values of a node and its children in a depth-first manner.
     */
    string snippet(pugi::xml_node node, unsigned int target_length) {
        string content = node.value();
        if (not content.empty()) {
            // Add a space if it's not empty to keep lines apart
            content += "\n";
        }
        if (content.length() >= target_length)
            return content;
        else 
            for (pugi::xml_node child : node) {
                content += snippet(child, target_length - content.length());
                if (content.length() >= target_length) break;
            }
        return content;
    }
    
    /**
     * Search for the first English abstract in the text, and try to get a
     * snippet of at least 100 characters from it.
     */
    string extract_english(string node_name) {
        for (auto abstract_node : root.children(node_name.c_str())) {
            auto lang = abstract_node.attribute("lang");
            if (not lang.empty() and string(lang.value()) == "EN") {
                return snippet(abstract_node, 100);
            }
        }
        return "";
    }

    /**
     * Collect all of the IPCR tags
     */
    string extract_tags() {
        regex tag_regex("\w+");
        smatch match;
        string out = "";
        //return snippet(root.first_element_by_path("bibliographic-data/technical-data/classifications-ipcr"), 1000);
        auto tags_parent = root.first_element_by_path("bibliographic-data/technical-data/classifications-ipcr");
        for (auto tag_node : tags_parent.children()) {
            string long_tag = tag_node.child_value();

            if (regex_search(long_tag, match, tag_regex)) {
                string short_tag = match[1].str();
                cout << short_tag << endl;
                ipcr_tags.push_back(short_tag);
                out += short_tag + " ";
            }
            out += long_tag + "\n";
        }
        return out;
    }

public:
    string abstract;
    string description;
    string id;
    string tags;
    vector<string> ipcr_tags;

    /**
     * Parse a document from file
     */
    Patent(string filename) {
        pugi::xml_parse_result result = doc.load_file(filename.c_str());
        //cout << "Load result: " << result.description() << endl;
        
        root = doc.child("patent-document");
        
        id = root.attribute("ucid").value();
        abstract = extract_english("abstract");
        description = extract_english("description");
        tags = extract_tags();
        
        /*
        cout << "ID: " << id << endl;
        cout << "Abstract: " << abstract << endl;
        cout << "Description: " << description << endl;
        */

        if (abstract.empty()
                //or description.empty()
                or id.empty())
            success = false;
        else
            success = true;
    }
    
    /**
     * Return whether any document was successfully parsed
     */
    bool empty() {
        return not success;
    }
};

/**
 * Parse each of the XML files and insert them into the database
 */
int main(int argc, const char * argv[]) { 
    string filename = "";
    if (argc < 3) {
        cerr << "Usage: parsexml <db> <document> [document ..]" << endl;
        return 2;
    }
    
    sqlite3_connection conn(argv[1]);
    conn.setbusytimeout(60000);
    conn.executenonquery("PRAGMA synchronous=0;");
    sqlite3_command insert_patent(conn, "INSERT OR REPLACE INTO patents (id, abstract, description, tags) VALUES (?, ?, ?, ?);");
    sqlite3_command insert_log(conn, "INSERT INTO log (id, filename, success) VALUES (?, ?, ?);");
    
    for (int i=2; i<argc; i++) {
        Patent p(argv[i]);
        insert_log.bind(1, p.id);
        insert_log.bind(2, argv[i]);

        if(p.empty()) {
            cout << "Parse failed on " << argv[i] << endl;
            insert_log.bind(3, 0);
        } else {
            insert_patent.bind(1, p.id);
            insert_patent.bind(2, p.abstract);
            insert_patent.bind(3, p.description);
            insert_patent.bind(4, p.tags);
            insert_patent.executenonquery();
            insert_log.bind(3, 1);
            cout << "Parse succeeded on " << argv[i] << endl;
        }
        insert_log.executenonquery();
    }
}
