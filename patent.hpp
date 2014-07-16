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
    string title;
    string abstract;
    string description;
    string id;
    string tags;
    string claims;
    string error_log;

    static Trie phrase_trie;

protected:
    /**
     * @brief Call this when finished to validate results
     */
    void validate();

public:
    Patent();
    // These accessors and mutators actually process the text
    // So they are not merely OO nannies

    string getTitle() const;
    void setTitle(const string &value);

    string getAbstract() const;
    void setAbstract(const string &value);

    string getDescription() const;
    void setDescription(const string &value);

    string getId() const;
    void setId(const string &value);

    string getTags() const;
    void appendTag(const string &value);

    string getClaims() const;
    void setClaims(const string &value);

    string getErrorLog() const;
    void appendErrorLog(const string &value);

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
    string sanitize(string content);


    /**
     * @brief Load a new list of no-break phrases
     * @param filename
     * @return the new phrases
     */
    void loadPhraseList(string filename);
};

#endif // PATENT_H
