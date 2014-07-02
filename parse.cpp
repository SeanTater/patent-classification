#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "cstring"
#include <boost/regex.hpp>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>

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
        boost::regex inline_references(R"lit((\([ ]*[0-9][0-9a-z,.; ]*\)))lit");
        content = boost::regex_replace(content, inline_references, "");

        boost::regex alpha_inline_references(R"lit((\([ ]*[A-Za-z]\)))lit");
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

    /**
     * @brief Split content on sentence boundaries
     * @param content
     * @return copy of content, with boundaries replaced
     *
     * Actually splitting the strings would result in serious overhead.
     * So instead, the boundaries are replaced with an easy-to-distinguish
     * character, ASCII unit separator \x1f (dec 31)
     */
    string split_sentences(string content) {
        content = boost::regex_replace(content, source_re, [](boost::smatch phrase){
            // To check: binary_search(source_phrases.begin(), source_phrases.end(), boost::to_lower_copy(phrase.str())) << endl;
            cout << "replaced " << phrase.str() << endl;
            return boost::replace_all_copy(phrase.str(), " ", "\xc2\xa0");
        });

        boost::regex sentence_breaks(R"lit(([\.!\?]( |^)))lit");
        return boost::regex_replace(content, sentence_breaks, "\1\x1f");
    }

    /**
     * @brief Load a new list of no-break phrases
     * @param filename
     * @return the new phrases
     */
    void load_phrase_list(string filename) {
        ifstream source(filename);
        string phrase;
        getline(source, phrase);
        while (not phrase.empty()) {
            boost::to_lower(phrase);
            Patent::source_phrases.push_back(phrase);
            Patent::cleaned_phrases.push_back(sanitize_for_regex(phrase));
            getline(source, phrase);
        }
        sort(source_phrases.begin(), source_phrases.end());
        sort(cleaned_phrases.begin(), cleaned_phrases.end());
    }

    /**
     * @brief Sanitize a regex as a literal
     * @param content
     * @return the new literal regex segment
     *
     * This came from Amber on StackOverflow
     */
    string sanitize_for_regex(string content) {
        const boost::regex esc("[\\^\\.\\$\\|\\(\\)\[\\]\\*\\+\\?\\/\\\\]");
        const string rep("\\\\\\1&");
        return boost::regex_replace(content, esc, rep, boost::match_default | boost::format_sed);
    }

public:
    string abstract;
    string description;
    string id;
    string tags;
    string error_log;
    static vector<string> source_phrases;
    static vector<string> cleaned_phrases;
    static boost::regex source_re;

    /**
     * Parse a document from file
     */
    Patent(string filename) {
        // TODO: Specify these filenames somewhere
        if (source_phrases.empty()) {
            load_phrase_list("source.list");
            // Is such a large RE a good idea?
            source_re = boost::regex("(?<= )(" + boost::join(cleaned_phrases, "|") + ")",
                                     boost::regex::icase);
        }

        pugi::xml_parse_result result = doc.load_file(filename.c_str());
        //cout << "Load result: " << result.description() << endl;

        root = doc.child("patent-document");

        id = root.attribute("ucid").value();
        abstract = split_sentences(extract_english("abstract"));
        description = split_sentences(extract_english("description"));
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

vector<string> Patent::source_phrases;
vector<string> Patent::cleaned_phrases;
boost::regex Patent::source_re;

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
