#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <string>
#include "cstring"
#include <boost/regex.hpp>
#include <vector>

using namespace std;
using namespace sqlite3x;

/**
 * @brief XML Patent Document Parser
 *
 * This class doesn't practice proper encapsulation.
 * Just read from the (non-null) string fields
 *  id, description, abstract, and tags
 */
class Patent {
private:
    pugi::xml_document doc;
    pugi::xml_node root;

    /**
     * @brief Clean the input of references
     * @param content
     * @return a cleaned copy
     */
    string sanitize(string content) {
        boost::regex inline_references(R"(\([ ]*[0-9][0-9a-z,.; ]*\))");
        content = boost::regex_replace(content, inline_references, "");

        boost::regex alpha_inline_references(R"(\([ ]*[A-Za-z]\))");
        return boost::regex_replace(content, alpha_inline_references, "");
    }

    /**
     * Try to extract a snippet of at least target_length characters from the
     * values of a node and its children in a depth-first manner.
     * @brief extract a snippet of at least target_length characters
     * @param node
     * @param target_length
     * @return a non-null string of approximately the target length
     */
    string snippet(pugi::xml_node node, unsigned int target_length) {
        string content = sanitize(node.value());
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
     * @brief Extract about 100 chars from the first English `node_name' tag
     * @param node_name
     * @return the extracted text
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
        string out = "";
        auto tags_parent = root.first_element_by_path("bibliographic-data/technical-data/classifications-ipcr");
        for (auto tag_node : tags_parent.children()) {
            string long_tag = tag_node.child_value();
            string short_tag;
            for (char c : long_tag) {
                if (c == ' ')
                    break;
                else
                    short_tag += c;
            }
            out += short_tag + " ";
        }
        return out;
    }

public:
    string abstract;
    string description;
    string id;
    string tags;
    string error_log;

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

        if (abstract.empty())
            error_log += "Missing abstract\n";
        if (id.empty())
            error_log += "Missing ID\n";
        if (tags.empty())
            error_log += "Could not find any IPCR tags.\n";
    }
    
    /**
     * Return whether any document was successfully parsed
     */
    bool empty() {
        return not error_log.empty();
    }
};

/**
 * Parse each of the XML files and insert them into the database
 */
int main(int argc, const char * argv[]) {

    // Arguments
    if (argc < 3) {
        cerr << "Usage: parsexml <db> <document> [document ..]" << endl;
        return 2;
    }
    
    // Database setup
    sqlite3_connection conn(argv[1]);
    conn.setbusytimeout(60000);
    conn.executenonquery("PRAGMA synchronous=0;");
    sqlite3_command insert_patent(conn, "INSERT OR REPLACE INTO patents (id, abstract, description, tags) VALUES (?, ?, ?, ?);");
    sqlite3_command insert_log(conn, "INSERT INTO log (id, filename, success, message) VALUES (?, ?, ?, ?);");
    

    // Load each patent file
    for (int i=2; i<argc; i++) {
        Patent p(argv[i]);
        insert_log.bind(1, p.id);
        insert_log.bind(2, argv[i]);

        if(p.empty()) {
            cout << "Parse failed on " << argv[i] << endl;
            insert_log.bind(3, 0);
            insert_log.bind(4, p.error_log);
        } else {
            insert_patent.bind(1, p.id);
            insert_patent.bind(2, p.abstract);
            insert_patent.bind(3, p.description);
            insert_patent.bind(4, p.tags);
            insert_patent.executenonquery();
            insert_log.bind(3, 1);
            insert_log.bind(4, "");
            cout << "Parse succeeded on " << argv[i] << endl;
        }
        insert_log.executenonquery();
    }
}
