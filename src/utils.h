#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Reads one entry per line. Returns an empty vector if the file can't be opened.
std::vector<std::string> get_array_from_file(const std::string &src);

// Locates a dataset by bare name ("music") or path ("data/music.txt"), trying
// both the repo root and build/ so the binary works from either directory.
// Returns an empty string if nothing matches.
std::string resolve_data_file(const std::string &name);

// Index of the first occurrence of `pat` in `txt`, or -1. Empty pattern -> 0.
int kmp_search(const std::string &pat, const std::string &txt);

// Every entry containing `pat` anywhere (KMP). Empty pattern matches all.
std::vector<std::string> search(const std::vector<std::string> &v, const std::string &pat);

// Naive baselines, kept for benchmarking against the trie.
std::vector<std::string> linear_search_pre(const std::vector<std::string> &v,
                                           const std::string &prefix);
bool linear_search_exact(const std::vector<std::string> &v, const std::string &str);

#endif // UTILS_H
