#include "patent.hpp"

Trie Patent::phrase_trie;

Patent::Patent()
{
    // TODO: Specify this filename somewhere
    if (phrase_trie.empty()) {
        loadPhraseList("source.list");
    }
}


string Patent::sanitize(string content) {
    boost::regex inline_references(R"lit((\([ ]*[0-9][0-9a-z,.; ]*\)))lit");
    content = boost::regex_replace(content, inline_references, "");

    boost::regex alpha_inline_references(R"lit((\([ ]*[A-Za-z]\)))lit");
    return boost::regex_replace(content, alpha_inline_references, "");
}

string Patent::splitSentences(string content) {
    // [start index, phrase]
    vector<pair<int, string> > replacements;

    for (auto c_it=content.begin(); c_it != content.end(); c_it++) {
        int c_i = c_it - content.begin();
        string phrase = phrase_trie.find(content, c_i);
        if (not phrase.empty()) {
            //cout << "matched " << content.substr(c_i, 50)  << " with " << phrase << endl;
            replacements.emplace_back(c_i, phrase);
        }
    }
    reverse(replacements.begin(), replacements.end());
    for (pair<int, string> replacement : replacements) {
        string& phrase = replacement.second;
        content.replace(replacement.first, phrase.length(),
                        boost::replace_all_copy(phrase, " ", "\xc2\xa0"));
    }
    boost::regex sentence_breaks(R"lit(([\.!\?]( |^)))lit");
    // ASCII unit separator (\x1f splits sentences)
    return boost::regex_replace(content, sentence_breaks, "\\1\x1f");
}

void Patent::loadPhraseList(string filename) {
    ifstream source(filename);
    string phrase;
    getline(source, phrase);
    while (not phrase.empty()) {
        boost::to_lower(phrase);
        Patent::phrase_trie.insert(phrase);
        getline(source, phrase);
    }
}

void Patent::validate() {
    if (abstract.empty())
        appendErrorLog("Missing abstract");
    if (id.empty())
        appendErrorLog("Missing ID");
    if (ipc.empty())
        appendErrorLog("Could not find any IPCR tags.");
}
bool Patent::success() {
    return error_log.empty();
}


string Patent::getDescription() const
{
    return description;
}
void Patent::setDescription(const string &value)
{
    description = splitSentences(sanitize(value));
}


string Patent::getId() const
{
    return id;
}
void Patent::setId(const string &value)
{
    id = value;
}


string Patent::getClaims() const
{
    return claims;
}
void Patent::setClaims(const string &value)
{
    claims = value;
}


string Patent::getErrorLog() const
{
    return error_log;
}
void Patent::appendErrorLog(const string &value)
{
    error_log += value + "\n";
}

string Patent::getTitle() const
{
    return title;
}
void Patent::setTitle(const string &value)
{
    title = splitSentences(sanitize(value));
}


string Patent::getAbstract() const
{
    return abstract;
}
void Patent::setAbstract(const string &value)
{
    abstract = splitSentences(sanitize(value));
}

/*
 * Classifications
 */

void Patent::appendClass(string &existing, const string& value) {
    if (existing.find(value) == string::npos)
        existing += value + " ";
}
