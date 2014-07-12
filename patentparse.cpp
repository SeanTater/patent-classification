#include "patentparse.hpp"

vector<string> PatentParse::source_phrases;
vector<string> PatentParse::cleaned_phrases;
boost::regex PatentParse::source_re;

class TrieNode;
class Trie {
private:
    TrieNode root {' '};
public:
    /**
     * @brief Find the longest prefix leaf or ""
     * @param text
     * @return
     */
    string find(const string& text) {
        return root.find(text, 0);
    }

    /**
     * @brief Insert a new leaf
     * @param text
     */
    void insert(const string& text) {
        root.insert(text, 0);
    }
};
class TrieNode {
    // Strings could be more efficient
    char value;
    bool leaf = false;
    vector<TrieNode> children;

    TrieNode(char value) {
        this->value = value;
    }

    /**
     * @brief Insert a string into the Trie
     * @param text
     * @param i
     * @return Whether a string was added
     */
    bool insert(const string& text, size_type i) {
        // 0 <= i < text.length()
        if (text.at(i) == value) {
            if (i == text.length() - 1)
                leaf = true;
            else if (i < text.length()) {
                // A DFS, in spite of looks (the char test is a linear search
                //  but it won't recurse in that case.)
                for (auto& candidate : children) {
                    if (candidate.insert(text, i+1))
                        return;
                }
                // No children with that text (but this is not a leaf)
                children.emplace_back(text.at(i+1));
                children.back().insert(text, i+1);
            }
        }
    }

    /**
     * @brief Find the longest prefix
     * @param text
     * @param i
     * @return A leaf, or an empty string. (no substrings)
     */
    string find(const string& text, size_type i) const {
        if (not text.empty() and text[i] == value) {
            for (auto& candidate : children) {
                string result = candidate.find(text, i+1);
                if (result)
                    return value + result;
            }
            if (leaf)
                return value;
        }
        return "";
    }
};

// public:

PatentParse::PatentParse(string filename) {
    // TODO: Specify these filenames somewhere
    if (source_phrases.empty()) {
        load_phrase_list("source.list");
        /* This oversized RE is a major performance loss
             * A binary search would be faster, and a trie faster still
             * But writing this is easier and I think the breakeven point
             * is maybe 10M patents */
        source_re = boost::regex("(?<= )(" + boost::join(cleaned_phrases, "|") + ")",
                                 boost::regex::icase);
    }

    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    //cout << "Load result: " << result.description() << endl;

    root = doc.child("patent-document");

    id = root.attribute("ucid").value();
    title = split_sentences(extract_english("invention-title", -1));
    abstract = split_sentences(extract_english("abstract", -1));
    description = split_sentences(extract_english("description"));
    tags = extract_tags();

    if (abstract.empty())
        error_log += "Missing abstract\n";
    if (id.empty())
        error_log += "Missing ID\n";
    if (tags.empty())
        error_log += "Could not find any IPCR tags.\n";
}

bool PatentParse::empty() {
    return not error_log.empty();
}


// private:

string PatentParse::sanitize(string content) {
    boost::regex inline_references(R"lit((\([ ]*[0-9][0-9a-z,.; ]*\)))lit");
    content = boost::regex_replace(content, inline_references, "");

    boost::regex alpha_inline_references(R"lit((\([ ]*[A-Za-z]\)))lit");
    return boost::regex_replace(content, alpha_inline_references, "");
}

string PatentParse::snippet(pugi::xml_node node, unsigned int target_length) {
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

string PatentParse::extract_english(string target_tag_name, unsigned int length) {
    pugi::xml_node target = root.find_node([&](pugi::xml_node& node){
        auto lang = node.attribute("lang");
        return node.name() == target_tag_name and not lang.empty() and string(lang.value()) == "EN";
    });
    return snippet(target, length);
}

string PatentParse::extract_tags() {
    string out = "";
    auto tags_parent = root.first_element_by_path("bibliographic-data/technical-data/classifications-ipcr");
    for (auto tag_node : tags_parent.children()) {
        string tag = string(tag_node.child_value()).substr(0, 3);
        if (out.find(tag) == string::npos)
            out += tag + " ";
    }
    return out;
}

/*string PatentParse::split_sentences(string content) {
    content = boost::regex_replace(content, source_re, [](boost::smatch phrase){
            // To check: binary_search(source_phrases.begin(), source_phrases.end(), boost::to_lower_copy(phrase.str())) << endl;
            cout << "replaced " << phrase.str() << endl;
            return boost::replace_all_copy(phrase.str(), " ", "\xc2\xa0");
    });
    boost::regex sentence_breaks(R"lit(([\.!\?]( |^)))lit");
    return boost::regex_replace(content, sentence_breaks, "\\1\x1f");
}*/

string PatentParse::split_sentences(string content) {
    // [start index, phrase index]
    vector<pair<int, int> > replacements;

    for (auto c_it=content.begin(); c_it != content.end(); c_it++) {
        int c_i = c_it - content.begin();
        if (*c_it != ' ' and (c_it == content.begin() or *(c_it-1) == ' ')) {
            // substr() -> unnecessary copy
            string suffix = boost::to_lower_copy(content.substr(c_i, 30));
            auto phrase_it = lower_bound(source_phrases.begin(), source_phrases.end(), suffix);
            if (phrase_it != source_phrases.end()) {
                if (phrase_it != source_phrases.begin())
                    cout << "maybe1 " << (*(phrase_it-1)) << " == " << suffix << endl;
                cout << "maybe2 " << (*(phrase_it+0)) << " == " << suffix << endl;
                cout << "maybe3 " << (*(phrase_it+1)) << " == " << suffix << endl;

                if (boost::istarts_with(suffix, (*phrase_it))) {
                    replacements.emplace_back(c_i, (*phrase_it).length());
                }
            }
        }
    }
    reverse(replacements.begin(), replacements.end());
    for (pair<int, int> replacement : replacements) {
        string& phrase = source_phrases.at(replacement.second);
        cout << "replacing " << phrase << endl;
        content.replace(replacement.first, phrase.length(),
                        boost::replace_all_copy(phrase, " ", "\xc2\xa0"));
    }
    /*content = boost::regex_replace(content, source_re, [](boost::smatch phrase){
            // To check: binary_search(source_phrases.begin(), source_phrases.end(), boost::to_lower_copy(phrase.str())) << endl;
            cout << "replaced " << phrase.str() << endl;
            return boost::replace_all_copy(phrase.str(), " ", "\xc2\xa0");
    });*/
    boost::regex sentence_breaks(R"lit(([\.!\?]( |^)))lit");
    return boost::regex_replace(content, sentence_breaks, "\\1\x1f");
}


void PatentParse::load_phrase_list(string filename) {
    ifstream source(filename);
    string phrase;
    getline(source, phrase);
    while (not phrase.empty()) {
        boost::to_lower(phrase);
        PatentParse::source_phrases.push_back(phrase);
        PatentParse::cleaned_phrases.push_back(sanitize_for_regex(phrase));
        getline(source, phrase);
    }
    sort(source_phrases.begin(), source_phrases.end());
    sort(cleaned_phrases.begin(), cleaned_phrases.end());
}

string PatentParse::sanitize_for_regex(string content) {
    const boost::regex esc("[\\^\\.\\$\\|\\(\\)\[\\]\\*\\+\\?\\/\\\\]");
    const string rep("\\\\\\1&");
    return boost::regex_replace(content, esc, rep, boost::match_default | boost::format_sed);
}
