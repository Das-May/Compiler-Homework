#include "regularExpression_processor.h"

static string PrintFATable(int row, int col, vector<vector<char>>& FA)
{
    string s;
    if(FA.size() == 0)
        s = "不存在";

    else if(FA.size() != row * col)
        s = "表格与指定行列数不匹配";

    else
    {
        // 打印表格
        int i = 0;
        for (auto& id_set : FA)
        {
            if (i % col == 0)
                s += '\n';

            for (auto& id : id_set)
            {
                s = s + to_string(id) + ' ';
            }
            s += '\t';

            i++;
        }
        s += '\n';
    }

    return s;
}

void RegularExpressionProcessor::SetNFA(vector<vector<char>> nfa)
{
    NFA = nfa;
}

string RegularExpressionProcessor::GetNFA()
{

}

