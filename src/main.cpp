#include "trie.h"
#include "utils.h"

#include <cstdio>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#ifdef _WIN32
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

namespace
{

    bool interactive()
    {
        return ISATTY(FILENO(stdin)) != 0;
    }

    void clear_screen()
    {
        if (interactive())
            std::cout << "\033[2J\033[H" << std::flush; // no shell-out to `clear`
    }

    void wait_for_enter()
    {
        if (!interactive())
            return;
        std::cout << "\n\npress Enter to continue.";
        std::string ignored;
        std::getline(std::cin, ignored);
    }

    void print_all(const std::vector<std::string> &words)
    {
        for (const auto &w : words)
            std::cout << w << "\n";
    }

    void print_menu()
    {
        std::cout << "------------------------\n"
                  << "MENU\n"
                  << "1. Exact search (trie)\n"
                  << "2. Prefix search (trie)\n"
                  << "3. Universal search (KMP substring)\n"
                  << "4. Delete a word\n"
                  << "5. Stats\n"
                  << "6. Benchmark: trie prefix vs linear scan\n"
                  << "0. Exit\n"
                  << "> ";
    }

    // Reads a menu choice. Returns false on EOF so the caller can exit instead
    // of spinning forever - the old `cin >> choice` loop never cleared the fail
    // state, so any bad input or a piped EOF meant an infinite menu.
    bool read_choice(int &choice)
    {
        std::string line;
        while (std::getline(std::cin, line))
        {
            try
            {
                std::size_t pos = 0;
                int value = std::stoi(line, &pos);
                if (pos == line.find_last_not_of(" \t\r") + 1)
                {
                    choice = value;
                    return true;
                }
            }
            catch (const std::exception &)
            {
                // fall through to the reprompt
            }
            std::cout << "Please enter a number from the menu.\n> ";
        }
        return false; // EOF
    }

} // namespace

int main(int argc, char **argv)
{
    std::string requested;

    if (argc > 1)
    {
        requested = argv[1];
    }
    else if (interactive())
    {
        std::cout << "Enter file to load data from ( music | movies | words ): ";
        if (!std::getline(std::cin, requested))
            return 0;
    }
    else
    {
        std::cerr << "usage: trie-search <data-file> [pattern]\n";
        return 1;
    }

    const std::string path = resolve_data_file(requested);
    if (path.empty())
    {
        std::cerr << "No dataset found for \"" << requested << "\".\n"
                  << "Try: music, movies, words, or a path to a text file.\n";
        return 1;
    }

    std::cerr << "loading " << path << " ...";
    std::vector<std::string> data = get_array_from_file(path);
    std::cerr << " done (" << data.size() << " entries).\n";

    if (data.empty())
    {
        std::cerr << "Dataset is empty, nothing to search.\n";
        return 1;
    }

    Trie trie;
    for (const auto &word : data)
        trie.insert(word);

    // Non-interactive one-shot mode: trie-search <file> <pattern>
    if (argc > 2)
    {
        print_all(search(data, argv[2]));
        return 0;
    }

    int choice = 0;
    do
    {
        print_menu();
        if (!read_choice(choice))
            break;
        if (choice == 0)
            break;

        std::string pattern;
        if (choice >= 1 && choice <= 4) // 6 prompts separately
        {
            std::cout << "Enter search text (leave empty to list all): ";
            if (!std::getline(std::cin, pattern))
                break;
        }

        switch (choice)
        {
        case 1:
            std::cout << (trie.search_exact(pattern) ? "String exists.\n" : "NOT found.\n");
            break;

        case 2:
            print_all(trie.search_pre(pattern));
            break;

        case 3:
            print_all(search(data, pattern));
            break;

        case 4:
            std::cout << (trie.remove(pattern) ? "Deleted.\n" : "Not in the trie.\n");
            break;

        case 5:
            std::cout << "words: " << trie.total_words()
                      << ", nodes: " << trie.total_nodes() << "\n";
            break;

        case 6:
        {
            std::cout << "Enter prefix to benchmark: ";
            std::string prefix;
            if (!std::getline(std::cin, prefix))
                return 0;
            std::cout << "trie:   ";
            const std::size_t via_trie = trie.search_pre(prefix).size();
            std::cout << "linear: ";
            const std::size_t via_scan = linear_search_pre(data, prefix).size();
            std::cout << (via_trie == via_scan
                              ? "both agree on the result count.\n"
                              : "MISMATCH between the two implementations.\n");
            break;
        }

        default:
            std::cout << "Unknown option.\n";
            break;
        }

        wait_for_enter();
        clear_screen();

    } while (true);

    return 0;
}
