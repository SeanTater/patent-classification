#ifndef PATENTPARSE_H
#define PATENTPARSE_H

#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace sqlite3x; // because it uses prefixes

/**
 * @brief XML Patent Document Parser
 *
 * This class doesn't practice proper encapsulation.
 * Just read from the (non-null) string fields
 *  id, description, abstract, and tags
 */
class PatentParse {
private:
    pugi::xml_document doc;
    pugi::xml_node root;
    static vector<string> source_phrases;
    static vector<string> cleaned_phrases;
    static boost::regex source_re;

    /**
     * @brief Clean the input of references
     * @param content
     * @return a cleaned copy
     */
    string sanitize(string content);

    /**
     * Try to extract a snippet of at least target_length characters from the
     * values of a node and its children in a depth-first manner.
     * @brief extract a snippet of at least target_length characters
     * @param node
     * @param target_length
     * @return a non-null string of approximately the target length
     */
    string snippet(pugi::xml_node node, unsigned int target_length);

    /**
     * @brief Extract about 100 chars from the first English `node_name' tag
     * @param node_name
     * @return the extracted text
     */
    string extract_english(string target_tag_name, unsigned int length=100);

    /**
     * Collect all of the IPCR tags
     */
    string extract_tags();

    /**
     * @brief Split content on sentence boundaries
     * @param content
     * @return copy of content, with boundaries replaced
     *
     * Actually splitting the strings would result in serious overhead.
     * So instead, the boundaries are replaced with an easy-to-distinguish
     * character, ASCII unit separator \x1f (dec 31)
     */
    string split_sentences(string content);

    /**
     * @brief Load a new list of no-break phrases
     * @param filename
     * @return the new phrases
     */
    void load_phrase_list(string filename);

    /**
     * @brief Sanitize a regex as a literal
     * @param content
     * @return the new literal regex segment
     *
     * This came from Amber on StackOverflow
     */
    string sanitize_for_regex(string content);

public:
    string title;
    string abstract;
    string description;
    string id;
    string tags;
    string error_log;

    /**
     * Parse a document from file
     */
    PatentParse(string filename);

    /**
     * Return whether any document was successfully parsed
     */
    bool empty();
};

#endif // PATENTPARSE_H
