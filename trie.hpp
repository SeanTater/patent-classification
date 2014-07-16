#ifndef TRIE_H
#define TRIE_H
#include<string>
#include<vector>
using namespace std;

///
/// Trie and it's implementation
///

class TrieNode {
public:
    // Strings could be more efficient
    string value;
    bool leaf;
    vector<TrieNode> children;

    TrieNode(string in);

    /**
     * @brief Insert a string into the Trie
     * @param text
     * @param i
     * @return Whether a string was added
     */
    void insert(const string& text, uint i);

    /**
     * @brief Find the longest prefix
     * @param text
     * @param i
     * @return A leaf, or an empty string. (no substrings)
     */
    string find(const string& text, uint i) const;

    /**
     * @return whether any strings are stored under this node
     */
    bool empty();
};

class Trie {
private:
    TrieNode root {""};
public:
    /**
     * @brief Find the longest prefix leaf or ""
     * @param text
     * @return
     */
    string find(const string& text);

    /**
     * @brief Find the longest prefix leaf or ""
     * @param text
     * @return
     */
    string find(const string& text, uint start_index);


    /**
     * @brief Insert a new leaf
     * @param text
     */
    void insert(string text);

    /**
     * @brief Check if the Trie is empty
     * @return true, if it is empty.
     */
    bool empty();
};


#endif // TRIE_H
