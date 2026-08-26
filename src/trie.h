#ifndef TRIE_H
#define TRIE_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class TrieNode
{
public:
    std::unordered_map<char, TrieNode *> children;
    bool isEndOfWord = false;
};

class Trie
{
public:
    Trie();
    ~Trie();

    // Nodes are raw-owned, so copying would double-free.
    Trie(const Trie &) = delete;
    Trie &operator=(const Trie &) = delete;

    void insert(const std::string &str);

    // Returns true if the word was present and removed.
    bool remove(const std::string &str);

    bool search_exact(const std::string &str) const;

    // All stored words starting with `prefix`. Empty prefix lists everything.
    std::vector<std::string> search_pre(const std::string &prefix) const;

    void draw(const TrieNode *current = nullptr, int depth = 0) const;

    std::size_t total_nodes() const { return total_nodes_; }
    std::size_t total_words() const { return total_words_; }

private:
    TrieNode *root_;
    std::size_t total_nodes_ = 0; // excluding the root
    std::size_t total_words_ = 0;

    bool contains_(const std::string &str) const; // lookup without timing output
    bool remove_(TrieNode *current, const std::string &str, std::size_t index);
    static void destroy_(TrieNode *node);
};

#endif // TRIE_H
