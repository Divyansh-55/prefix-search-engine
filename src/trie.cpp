#include "trie.h"
#include "timing.h"

#include <iostream>

Trie::Trie() : root_(new TrieNode()) {}

Trie::~Trie()
{
    destroy_(root_);
    root_ = nullptr;
}

void Trie::destroy_(TrieNode *node)
{
    if (!node)
        return;
    for (auto &entry : node->children)
        destroy_(entry.second);
    delete node;
}

void Trie::insert(const std::string &str)
{
    TrieNode *current = root_;

    for (char ch : str)
    {
        auto it = current->children.find(ch);
        if (it == current->children.end())
        {
            it = current->children.emplace(ch, new TrieNode()).first;
            ++total_nodes_;
        }
        current = it->second;
    }

    if (!current->isEndOfWord)
    {
        current->isEndOfWord = true;
        ++total_words_;
    }
}

bool Trie::contains_(const std::string &str) const
{
    const TrieNode *current = root_;

    for (char ch : str)
    {
        auto it = current->children.find(ch);
        if (it == current->children.end())
            return false;
        current = it->second;
    }

    return current->isEndOfWord;
}

bool Trie::search_exact(const std::string &str) const
{
    auto start = Clock::now();

    const TrieNode *current = root_;
    bool found = true;

    for (char ch : str)
    {
        auto it = current->children.find(ch);
        if (it == current->children.end())
        {
            found = false;
            break;
        }
        current = it->second;
    }

    if (found)
        found = current->isEndOfWord;

    // Timing now covers the whole lookup, including the miss path.
    report(found ? 1 : 0, ms_since(start));
    return found;
}

static void get_words_dfs(const TrieNode *current, const std::string &pre,
                          std::vector<std::string> &results)
{
    if (!current)
        return;

    if (current->isEndOfWord)
        results.push_back(pre);

    for (const auto &entry : current->children)
        get_words_dfs(entry.second, pre + entry.first, results);
}

std::vector<std::string> Trie::search_pre(const std::string &prefix) const
{
    auto start = Clock::now();

    const TrieNode *current = root_;
    std::vector<std::string> results;

    for (char ch : prefix)
    {
        auto it = current->children.find(ch);
        if (it == current->children.end())
        {
            report(0, ms_since(start));
            return results;
        }
        current = it->second;
    }

    get_words_dfs(current, prefix, results);

    report(results.size(), ms_since(start));
    return results;
}

bool Trie::remove(const std::string &str)
{
    if (!contains_(str))
        return false;

    remove_(root_, str, 0);
    --total_words_;
    return true;
}

bool Trie::remove_(TrieNode *current, const std::string &str, std::size_t index)
{
    if (index == str.size())
    {
        if (!current->isEndOfWord)
            return false;

        current->isEndOfWord = false;
        return current->children.empty();
    }

    // find(), not operator[]: operator[] silently inserted a null child here,
    // corrupting the trie on every failed delete.
    auto it = current->children.find(str[index]);
    if (it == current->children.end())
        return false;

    if (remove_(it->second, str, index + 1))
    {
        delete it->second;
        current->children.erase(it);
        --total_nodes_;
        return current->children.empty() && !current->isEndOfWord;
    }

    return false;
}

void Trie::draw(const TrieNode *current, int depth) const
{
    if (!current)
        current = root_;

    for (const auto &entry : current->children)
    {
        for (int i = 0; i < depth; ++i)
            std::cout << "_ ";
        std::cout << entry.first << "\n";
        draw(entry.second, depth + 1);
    }
}
