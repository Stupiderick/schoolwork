/**
 * @file cartalk_puzzle.cpp
 * Holds the function which solves a CarTalk puzzler.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

#include <fstream>

#include "cartalk_puzzle.h"

using namespace std;

/**
 * Solves the CarTalk puzzler described here:
 * http://www.cartalk.com/content/wordplay-anyone.
 * @return A vector of "StringTriples" (a typedef'd std::tuple, see top of
 * cartalk_puzzle.h). Returns an empty vector if no solutions are found.
 * @param d The PronounceDict to be used to solve the puzzle.
 * @param word_list_fname The filename of the word list to be used.
 */
vector<StringTriple> cartalk_puzzle(PronounceDict d,
                                    const string& word_list_fname)
{
    vector<StringTriple> out;
    ifstream wordsFile(word_list_fname);
    string word;

    if (wordsFile.is_open()) {
        while (getline(wordsFile, word)) {
            if (word.length() == 5) {
                string r1 = word.substr(1);  //word removed first letter
                string r2 = word.substr(0, 1);  //word removed second letter
                string temp = word.substr(2, word.size() - 2);
                r2.append(temp);

                if (d.homophones(r1, word)
                    && d.homophones(r2, word)
                    && d.homophones(r1, r2)) {
                        StringTriple theTriple = make_tuple(word, r1, r2);
                        out.push_back(theTriple);
                }
            }
        }
    }

    return out;
}
