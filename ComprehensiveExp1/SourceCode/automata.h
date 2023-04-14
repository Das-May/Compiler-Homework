#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <vector>
#include <set>

using namespace std;

class Automata
{
public:
    /**
     * @brief Generate DFA Table by process NFA Table
     * @param NFA is a 2D Table represented by a 1D array, every table cell is a vector<int>
     * @return DFA Table is also a 2D Table represented by a 1D array, every table cell is a vector<int>, and the DFA[0][0] is the number of row, DFA[0][1]i is the col, other element of DFA[0] are the ID numbers of end nodes of DFA
     */
    static vector<vector<int>> GetDFA(const vector<vector<int>>& NFA);

    /**
     * @brief Generate minimized DFA Table by process DFA Table
     * @param DFA is a 2D Table represented by a 1D array, every table cell is a vector<int>, and the DFA[0][0] is the number of row, DFA[0][1]i is the col, other element of DFA[0] are the ID numbers of end nodes of DFA
     * @return MinDFA Table is also a 2D Table which is same as DFA
     */
    static vector<vector<int>> GetMinDFA(const vector<vector<int>>& DFA);

private:
    static vector<vector<int>> GenerateDFA(const vector<vector<int>>& NFA);
    static void SimplifyDFA(vector<vector<int>>& DFA);
    static int FindSetIndex(const vector<set<int>>& StatusGroup, int Target);
};

#endif // AUTOMATA_H
