#ifndef PATENT_H
#define PATENT_H
#include <trie.hpp>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>
using namespace std;

/**
 * @brief The Patent base class
 */
class Patent {
private:
    // Metadata
    string id;
    string error_log;

    // Bibliographic data
    string title;
    string abstract;
    string description;
    string claims;

    // Used to store phrases to remove
    static Trie phrase_trie;

public:

    // Classifications
    // These are easier to handle public.
    // Use appendClass with them.
    string ipc;
    string uspc;
    string cpc;
    string ecla;

    Patent();
    // These accessors and mutators actually process the text
    // So they are not merely OO nannies

    // Metadata
    string getId() const;
    void setId(const string &value);

    string getErrorLog() const;
    void appendErrorLog(const string &value);


    // Bibliographic data
    string getTitle() const;
    void setTitle(const string &value);

    string getAbstract() const;
    void setAbstract(const string &value);

    string getDescription() const;
    void setDescription(const string &value);

    string getClaims() const;
    void setClaims(const string &value);

    // Classifications
    void appendClass(string& existing, const string &value);

    /**
     * @return Whether the results were valid to add to the DB
     */
    bool success();

    /**
     * @brief Split content on sentence boundaries
     * @param content
     * @return copy of content, with boundaries replaced
     *
     * Actually splitting the strings would result in serious overhead.
     * So instead, the boundaries are replaced with an easy-to-distinguish
     * character, ASCII unit separator \x1f (dec 31)
     */
    string splitSentences(string content);


    /**
     * @brief Clean the input of references
     * @param content
     * @return a cleaned copy
     */
    static string sanitize(string content);


    /**
     * @brief Load a new list of no-break phrases
     * @param filename
     * @return the new phrases
     */
    void loadPhraseList(string filename);

    /**
     * @brief Call this when finished to validate results
     */
    void validate();
};

#endif // PATENT_H
