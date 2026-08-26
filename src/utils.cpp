#include "utils.h"
#include "timing.h"

#include <fstream>
#include <iostream>

std::vector<std::string> get_array_from_file(const std::string &src)
{
    std::vector<std::string> words;
    std::ifstream file(src);

    if (!file)
    {
        std::cerr << "\nCan't open file " << src << " - skipping.\n";
        return words;
    }

    // while (getline(...)), not while (!file.eof()): the old loop read one
    // phantom line past the end.
    std::string word;
    while (std::getline(file, word))
    {
        if (!word.empty() && word.back() == '\r') // tolerate CRLF files
            word.pop_back();
        if (!word.empty())
            words.push_back(word);
    }

    return words;
}

std::string resolve_data_file(const std::string &name)
{
    std::vector<std::string> candidates;

    if (name.find('/') != std::string::npos || name.size() > 4)
        candidates.push_back(name); // looks like a path already

    candidates.push_back("data/" + name + ".txt");
    candidates.push_back("../data/" + name + ".txt");
    candidates.push_back(name + ".txt");

    for (const auto &path : candidates)
    {
        std::ifstream probe(path);
        if (probe)
            return path;
    }

    return {};
}

static void compute_lps(const std::string &pat, std::vector<int> &lps)
{
    lps[0] = 0;
    std::size_t len = 0;
    std::size_t i = 1;

    while (i < pat.size())
    {
        if (pat[i] == pat[len])
        {
            lps[i++] = static_cast<int>(++len);
        }
        else if (len != 0)
        {
            len = static_cast<std::size_t>(lps[len - 1]);
        }
        else
        {
            lps[i++] = 0;
        }
    }
}

int kmp_search(const std::string &pat, const std::string &txt)
{
    // The crash: the old code declared `int lps[M]` with M == 0 for an empty
    // pattern, then wrote lps[0]. An empty pattern trivially matches at 0.
    if (pat.empty())
        return 0;
    if (pat.size() > txt.size())
        return -1;

    std::vector<int> lps(pat.size()); // heap-allocated, not a zero-length VLA
    compute_lps(pat, lps);

    std::size_t i = 0; // index into txt
    std::size_t j = 0; // index into pat

    while (i < txt.size())
    {
        if (pat[j] == txt[i])
        {
            ++i;
            ++j;
            if (j == pat.size())
                return static_cast<int>(i - j);
        }
        else if (j != 0)
        {
            j = static_cast<std::size_t>(lps[j - 1]);
        }
        else
        {
            ++i;
        }
    }

    return -1;
}

std::vector<std::string> search(const std::vector<std::string> &v, const std::string &pat)
{
    std::vector<std::string> results;
    auto start = Clock::now();

    for (const auto &str : v)
    {
        if (kmp_search(pat, str) != -1)
            results.push_back(str);
    }

    report(results.size(), ms_since(start));
    return results;
}

std::vector<std::string> linear_search_pre(const std::vector<std::string> &v,
                                           const std::string &prefix)
{
    std::vector<std::string> results;
    auto start = Clock::now();

    for (const auto &s : v)
    {
        // compare() bounds-checks; the old loop read s[i] past the end
        // whenever the prefix was longer than the entry.
        if (s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0)
            results.push_back(s);
    }

    report(results.size(), ms_since(start));
    return results;
}

bool linear_search_exact(const std::vector<std::string> &v, const std::string &str)
{
    auto start = Clock::now();

    bool found = false;
    for (const auto &s : v)
    {
        if (s == str)
        {
            found = true;
            break;
        }
    }

    report(found ? 1 : 0, ms_since(start));
    return found; // the old version always returned false
}
