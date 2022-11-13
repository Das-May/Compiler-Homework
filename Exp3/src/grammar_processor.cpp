#include "grammar_processor.h"

// 初始化函数，创建字符字典和文法规则
GrammarProcessor::GrammarProcessor(char *c)
{
    AddVt('$');

    Rule temp_r;
    for (; *c != 0; c++)
    {
        AddVn(*c);
        temp_r.left = value2key(*c);

        c += 3;

        temp_r.right.clear();
        for(; *c!='\n' && *c != 0; c++)
        {
            if(*c >= 'A' && *c <= 'Z')
            {
                temp_r.right.push_back(n_num);
                AddVn(*c);
            }
            else
            {
                temp_r.right.push_back(t_num);
                AddVt(*c);
            }
        }
        grammar.push_back(temp_r);
    }
}


// 打印单条文法
string GrammarProcessor::PrintRule(Rule r)
{
    string r_string;
    r_string = v[r.left];
    r_string = r_string + "->";
    for(auto& j : r.right)
    {
        r_string += v[j];
    }
    r_string += '\n';
    return r_string;
}

// 打印全部文法
string GrammarProcessor::PrintGrammar()
{
    string g_string;
    for(auto& i : grammar)
    {
        g_string += PrintRule(i);
    }
    return g_string;
}

// 添加非终结符
void GrammarProcessor::AddVn(char c)
{
    v[n_num] = c;     // 终结符号
    vn.push_back(n_num);
    n_num++;
}

// 添加终结符
void GrammarProcessor::AddVt(char c)
{
    v[t_num] = c;
    t_num++;
}

// 根据值查找出字典中的键
int GrammarProcessor::value2key(char c)
{
    for(auto & i : v)
    {
        if(i.second == c)
            return i.first;
    }
    cout << "---Error: Can not find key from value!---" << endl;
    return 0;
}






