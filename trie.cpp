#include "trie.hpp"

TrieNode::TrieNode(string in) {
    this->value = in;
    this->leaf = false;
}

void TrieNode::insert(const string &text, uint i) {
    // 0 <= i < text.length()
    if (i == text.length()) {
        leaf = true;
    } else if (i < text.length()) {
        // A DFS, in spite of looks (the char test is a linear search
        //  but it won't recurse in that case.)
        for (auto& candidate : children) {
            if (tolower(candidate.value.at(0)) == tolower(text.at(i))) {
                candidate.insert(text, i+1);
                return;
            }
        }
        // No children with that text (but this is not a leaf)
        children.emplace_back(text.substr(i, 1));
        children.back().insert(text, i+1);
    }
}

string TrieNode::find(const string &text, uint i) const {
    if (i < text.length()) {
        for (auto& candidate : children) {
            if (tolower(text.at(i)) == tolower(candidate.value.at(0))) {
                string result = candidate.find(text, i+1);
                if (not result.empty())
                    return value + result;
            }
        }
    }
    if (leaf) {
        return value;
    }
    return "";
}

bool TrieNode::empty() {
    return (not leaf) and children.empty();
}


string Trie::find(const string &text) {
    return root.find(text, 0);
}

string Trie::find(const string &text, uint start_index) {
    return root.find(text, start_index);
}

void Trie::insert(string text) {
    root.insert(text, 0);
}

bool Trie::empty() {
    return root.empty();
}
