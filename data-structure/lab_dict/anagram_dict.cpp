/**
 * @file anagram_dict.cpp
 * Implementation of the AnagramDict class.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

#include "anagram_dict.h"

#include <algorithm> /* I wonder why this is included... */
#include <fstream>

using std::string;
using std::vector;
using std::ifstream;

/**
 * Constructs an AnagramDict from a filename with newline-separated
 * words.
 * @param filename The name of the word list file.
 */
AnagramDict::AnagramDict(const string& filename)
{
    ifstream anagram_dict_file(filename);
    string word;

    if (anagram_dict_file.is_open()) {
        while (getline(anagram_dict_file, word)) {
            string sortedWord = word;
            string originalWord = word;
            std::sort(sortedWord.begin(), sortedWord.end());

            auto lookup = dict.find(sortedWord);
            if (lookup == dict.end()) {
                dict[sortedWord].push_back(originalWord);
            } else {
                lookup->second.push_back(originalWord);
            }
        }
    }
}

/**
 * Constructs an AnagramDict from a vector of words.
 * @param words The vector of strings to be used as source words.
 */
AnagramDict::AnagramDict(const vector<string>& words)
{
    for (unsigned long i = 0; i < words.size(); i++) {
        string sortedWord = words.at(i);
        string originalWord = words.at(i);
        std::sort(sortedWord.begin(), sortedWord.end());

        auto lookup = dict.find(sortedWord);
        if (lookup == dict.end()) {
            dict[sortedWord].push_back(originalWord);
        } else {
            lookup->second.push_back(originalWord);
        }
    }
}

/**
 * @param word The word to find anagrams of.
 * @return A vector of strings of anagrams of the given word. Empty
 * vector returned if no anagrams are found or the word is not in the
 * word list.
 */
vector<string> AnagramDict::get_anagrams(const string& word) const
{
    string sortedWord = word;
    std::sort(sortedWord.begin(), sortedWord.end());

    auto lookup = dict.find(sortedWord);
    if (lookup == dict.end()) {
        return vector<string>();
    }

    if (lookup->second.size() == 1) {
        return vector<string>();
    }

    vector<string> getAnagrams;
    for (size_t i = 0; i < lookup->second.size(); i++) {
        string oneAnagram = lookup->second.at(i);
        getAnagrams.push_back(oneAnagram);
    }

    return getAnagrams;
}

/**
 * @return A vector of vectors of strings. Each inner vector contains
 * the "anagram siblings", i.e. words that are anagrams of one another.
 * NOTE: It is impossible to have one of these vectors have less than
 * two elements, i.e. words with no anagrams are ommitted.
 */
vector<vector<string>> AnagramDict::get_all_anagrams() const
{
    auto findup = dict.begin();
    vector<vector<string>> getAllAnagrams;

    while (findup != dict.end()) {
        vector<string> oneWord = get_anagrams(findup->first);
        if (!oneWord.empty()) {
            getAllAnagrams.push_back(oneWord);
        }

        findup++;
    }

    return getAllAnagrams;
}
