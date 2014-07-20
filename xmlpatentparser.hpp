#ifndef PATENTPARSE_H
#define PATENTPARSE_H

#include <pugixml.hpp>
#include <sqlite3x-master/sqlite3x.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <patent.hpp>

using namespace std;
using namespace sqlite3x; // because it uses prefixes


/**
 * @brief Patent Dialect (determines some tag names, attributes)
 */
enum PatentDialect {GOOGLE, CLEF};

/**
 * @brief XML Patent Document Parser
 *
 * This class doesn't practice proper encapsulation.
 * Just read from the (non-null) string fields
 *  id, description, abstract, and tags
 */
class XMLPatentParser {
private:
    /* This exists to keep the document around until all the document's
     * PatentParse's are deleted.
     */
    shared_ptr<pugi::xml_document> doc;
    pugi::xml_node root;
    PatentDialect dialect;
    Patent pat;

    /**
     * Try to extract a snippet of at least target_length characters from the
     * values of a node and its children in a depth-first manner.
     * @brief extract a snippet of at least target_length characters
     * @param node
     * @param target_length
     * @return a non-null string of approximately the target length
     */
    string snippet(pugi::xml_node node, unsigned int target_length, const string separator="\n");

    /**
     * @brief Extract about 100 chars from the first English `node_name' tag
     * @param node_name
     * @return the extracted text
     */
    string extractEnglish(string target_tag_name, unsigned int length=100);

    /**
     * @brief Extract IPCR classifications
     */
    void extractIPC();

    /**
     * @brief Extract USPC classifications
     */
    void extractUSPC();

    /**
     * @brief Extract ECLA classifications
     */
    void extractECLA();

    /**
     * @brief Extract patent claims, with delimiters
     */
    string extractClaims();

public:

    /**
     * Parse a document from file
     */
    XMLPatentParser(shared_ptr<pugi::xml_document> doc, pugi::xml_node root, PatentDialect dialect);

    /**
     * @brief Parse (possibly multiple) documents from an XML file
     * @param filename
     * @return a list of patent parses
     * In the case that your XML file is broken (or not XML), you will get an empty list.
     */
    static vector<Patent> parseXML(string filename);

    const Patent getPatent() const;
};

#endif // PATENTPARSE_H
