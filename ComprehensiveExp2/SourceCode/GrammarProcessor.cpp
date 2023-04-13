#include "GrammarProcessor.h"

#include <algorithm>
#include <iostream>

// 初始化函数，创建字符字典和文法规则
GrammarProcessor::GrammarProcessor(char *GrammarBuffer)
{
    Rule TempRule;
    string TempWord = "";
    int LatestID = 0;
    while(*GrammarBuffer != 0 && *GrammarBuffer != '\n')
    {
        // 读取左部
        TempWord = "";
        while(*GrammarBuffer != ' ')
        {
            TempWord += *GrammarBuffer;
            GrammarBuffer++;
        }
        int ID = Word2ID(TempWord);
        if(ID == -1)
        {
            ID2Word[LatestID] = TempWord;
            TempRule.Left = LatestID;
            LatestID++;
        }
        else
        {
            TempRule.Left = ID;
        }

        // 跳过->
        bool end = false;
        while(*GrammarBuffer == ' ' || *GrammarBuffer == '-' || *GrammarBuffer == '>')
        {
            if(*GrammarBuffer == '>')
            {
                end = true;
            }
            else if(end && *GrammarBuffer == '-')//防止吞并文法符号，例如-，--，-=
            {
                break;
            }
            GrammarBuffer++;
        }

        // 读取右部
        TempRule.Right.clear();
        while(*GrammarBuffer != '\n' && *GrammarBuffer != 0)
        {
            TempWord = "";
            while(*GrammarBuffer != '\n' && *GrammarBuffer != ' ' &&
                  *GrammarBuffer != 0 && *GrammarBuffer != '\r')
            {
                TempWord += *GrammarBuffer;
                GrammarBuffer++;
            }

            int ID = Word2ID(TempWord);
            if(ID == -1)
            {
                ID2Word[LatestID] = TempWord;
                TempRule.Right.push_back(LatestID);
                LatestID++;
            }
            else
            {
                TempRule.Right.push_back(ID);
            }

            // 跳过空格和间隔
            while(*GrammarBuffer == ' ' || *GrammarBuffer == '|' || *GrammarBuffer == '\r')
            {
                GrammarBuffer++;
            }
        }

        Grammar.push_back(TempRule);

        // 跳过换行符
        if(*GrammarBuffer != 0)
        {
            GrammarBuffer++;
        }
    }
    OrganizeID2Word();
}

void GrammarProcessor::OrganizeID2Word()
{
    bool IsNonTerminal[200];
    memset(IsNonTerminal, false, sizeof(IsNonTerminal));
    for(Rule& grammar : Grammar)
    {
        IsNonTerminal[grammar.Left] = true;
    }

    map<int, string> TempID2Word;
    NonterminalLatestID = 0;
    TempID2Word[100] = "#"; // epslion
    TempID2Word[101] = "$";
    TerminalLatestID = 102;

    map<int, int> UpdateID;

    for(auto pair : ID2Word)
    {
        if(IsNonTerminal[pair.first] == true)
        {
            TempID2Word[NonterminalLatestID] = pair.second;
            UpdateID[pair.first] = NonterminalLatestID;
            NonterminalLatestID++;
        }
        else
        {
            TempID2Word[TerminalLatestID] = pair.second;
            UpdateID[pair.first] = TerminalLatestID;
            TerminalLatestID++;
        }

    }

    ID2Word = TempID2Word;

    for(auto& grammar : Grammar)
    {
        grammar.Left = UpdateID[grammar.Left];
        for(auto& right : grammar.Right)
        {
            right = UpdateID[right];
        }
    }

    int i  = 0;
}

#pragma region "消除有害规则"{

void GrammarProcessor::RemoveHarmfulRules()
{
    list<Rule>::iterator it = Grammar.begin();
    int size = Grammar.size();
    for(int i = 0; i < size; i++, it++)
    {
        if((*it).Right.size() == 1 && (*it).Left == (*it).Right[0]) // U->U
        {
            it = Grammar.erase(it);
            it--;
        }
    }
}

void GrammarProcessor::RemoveUnreachableRules()
{
    bool Reachable[200];
    memset(Reachable, 0, sizeof(Reachable));
    Reachable[0] = true; // 将左部开始符号放入集合中
    for(auto& grammar : Grammar)
    {
        if(Reachable[grammar.Left] == true) // 若左部在集合中
        {
            for(auto& right : grammar.Right)
            {
                if(IsNonterminal(right)) // 对于右部的每个非终结符号
                {
                    Reachable[right] = true; // 放入“可到达集合”中
                }
            }
        }

    }

    list<Rule>::iterator it = Grammar.begin();
    int size = Grammar.size();
    for(int i = 0; i < size; i++, it++)
    {
        if(Reachable[(*it).Left] == false)
        {
            it = Grammar.erase(it);
            it--;
        }
    }
}

// 删除不终止规则
bool GrammarProcessor::RemoveUnterminableRules_sub(int NonterminalID, int depth)
{
    if(TempSet[NonterminalID] != -1)
        return TempSet[NonterminalID];

    if(depth == 5)
        return false;

    bool Processed = false;
    bool Endable;
    for(auto& grammar : Grammar)
    {
        Endable = true;
        if(grammar.Left == NonterminalID)    // 遍历以A为左部的每个文法
        {
            Processed = true;
            for(int RightID : grammar.Right)
            {
                if(IsNonterminal(RightID))
                {
                    if(RightID == NonterminalID)// 满足A->αAβ形式
                    {
                        Endable = false;
                        continue;
                    }
                    else//存在其它非终结符
                    {
                        Endable = RemoveUnterminableRules_sub(RightID, depth + 1);
                        if(Endable == true)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if(Processed == true) //（剪枝操作）
        {
            break;
        }
    }
    TempSet[NonterminalID] = Endable && Processed;
    return Endable;
}

// 删除不可终止规则
void GrammarProcessor::RemoveUnterminableRules()
{
    memset(TempSet, -1, sizeof(TempSet)); // 意为Unterminable，是否可终止
    RemoveUnterminableRules_sub(0, 0);
    for(int ID = 0; ID < NonterminalLatestID; ID++)
    {
        if(TempSet[ID] == 0)    // 如果该字符A被判为不可终止
        {
            list<Rule>::iterator it = Grammar.begin();
            int size = Grammar.size();
            for(int j = 0; j < size; j++, it++) // 遍历所有文法
            {
                cout << "(*it).left" << (*it).Left << endl;
                if((*it).Left == ID)         // 左部含有该字符A
                {
                    it = Grammar.erase(it);
                    it--;
                }
                else
                {
                    for(int RightID : (*it).Right)
                    {
                        if(RightID == ID)  // 右部含有该字符A
                        {
                            it = Grammar.erase(it);
                            it--;
                            break;
                        }
                    }
                }
            }

            ID2Word.erase(ID2Word.find(ID));//在字典中也移除A
        }
    }
}

#pragma endregion }

string GrammarProcessor::SimplifyGrammar()
{
    RemoveHarmfulRules();
    RemoveUnreachableRules();
    RemoveUnterminableRules();
    return PrintGrammar();
}
/*
#pragma region "获取First(x)"{
void GrammarProcessor::GetFirst_sub(int x)
{
    bool tag = false;
    for(auto& g : grammar)
    {
        if(g.left == x)    // 遍历以X为左部的每个文法
        {
            tag = true;
            int a = g.right[0]; // 取文法右部第一个字符假设为a
            if( isVt(a) )     // 若a是终结符或ε
            {
                bool repeat = false;
                for(list<int>::iterator it = first.begin(); it != first.end(); it++)
                {
                    if((*it) == a)
                    {
                        repeat = true;
                        break;
                    }
                }
                if(repeat == false)
                    first.push_back(a);
            }
            else if(a>0 && a!=x)    // 若为非终结符 并且不是本身（避免进入死循环）
            {
                GetFirst_sub(a);    // 则递归进入GetFirst(int a)
            }

        }
        else if(tag == true) //（剪枝操作）
        {
            break;
        }
    }
}

list<int>& GrammarProcessor::GetFirst(int x)
{
    first.clear();
    if(isVn(x))
        GetFirst_sub(x);
    else
        first.push_back(x);
    return first;
}

string GrammarProcessor::GetFirst()
{
    string allFirst = "";
    vector<int> temp_vector;
    for(int i = 0; i < NonterminalLatestID; i++)
    {
        if(v[i] != 0)
        {
            allFirst += "First(";
            allFirst += v[i];
            allFirst += ")={" ;
            temp_vector.clear();
            for(auto& element : GetFirst(i))
            {
                if(find(temp_vector.begin(), temp_vector.end(), element) == false)
                    temp_vector.push_back(element);
            }
            for(auto& element : temp_vector)
            {
                allFirst += v[element];
                allFirst += ", ";
            }
            allFirst += "}\n";
        }
    }
    return allFirst;
}
#pragma endregion }

#pragma region "获取Follow(x)"{
list<int> GrammarProcessor::GetFollow(int x)
{
    list<int> follow;
    if(x == 0) // ①当x是文法的开始符号
    {
        follow.push_back(value2key('$')); // 加入$
    }

    for(auto& g : grammar) // 遍历每个文法
    {
        if(x == g.right[ g.right.size() - 1 ])   // ②当x是最右部的时候
        {
            follow.push_back(value2key('$')); // 加入$

            // ④如果存在一个产生式A→αB，那么follow（B）包含follow（A），注意A≠B
            if(g.left != x)
            {
                for(auto& f : GetFollow(g.left))
                {
                    follow.push_back(f);
                }
            }
        }

        bool tag = false;
        for(auto& r : g.right)
        {
            if(r == x)
            {
                tag = true;
            }
            else if(tag == true)// ③此时已遍历到β，即存在一个产生式A→αBβ
            {
                list<int> kkk = GetFirst(r);
                for(auto& f : kkk)
                {
                    if(f != value2key('#'))    // 那么follow（B）包含first（β）-ε
                    {
                        follow.push_back(f);
                    }
                    else if(g.left != x)    // ④存在产生式A→αBβ且first（β）包含ε，注意A≠B
                    {
                        for(auto& f : GetFollow(g.left)) // 那么follow（B）包含follow（A）
                        {
                            follow.push_back(f);
                        }
                    }
                }
            }
        }

    }
    return follow;
}

string GrammarProcessor::GetFollow()
{
    string allFollow = "";
    vector<int> temp_vector;
    for(int i = 0; i < NonterminalLatestID; i++)
    {
        if(v[i] != 0)
        {
            allFollow += "First(";
            allFollow += v[i];
            allFollow += ")={" ;
            temp_vector.clear();
            for(auto& element : GetFollow(i))
            {
                if(find(temp_vector.begin(), temp_vector.end(), element) == false)
                    temp_vector.push_back(element);
            }
            for(auto& element : temp_vector)
            {
                allFollow += v[element];
                allFollow += ", ";
            }
            allFollow += "}\n";
        }
    }
    return allFollow;
}
#pragma endregion }

// 消除左公因子
string GrammarProcessor::RemoveLeftCommonFactor()
{
    list<list<Rule>::iterator> leftEqual2i;
    int i = 0;
    map<list<int>, int> m;
    int n = 0;

    list<Rule>::iterator it = grammar.begin();
    for(; it != grammar.end(); it++)// 遍历每个非终结符号，设当前符号为Vni
    {
        if((*it).left == i)    // 遍历以Vni为左部的每个文法
        {
            leftEqual2i.push_back(it); // 装入容器list<Rule>中
        }
        else    // 左部i改变了，说明上一个遍历完了
        {
            // 处理上一个非终结符号
            if(leftEqual2i.size() != 1) // 若个数n≠1
            {
                // 定义字典map<vector, int>，和数组union[ n ]（初始化为0）
                m.clear();
                memset(temp_set, 0, sizeof (temp_set) );
                n = 1;// !!!
                int tag = false;

                for(auto & rule:leftEqual2i ) // 遍历以Vni为左部的文法
                {
                    int right = (*rule).right[0]; // 右部第一个字符
                    list<int> f = GetFirst(right);
                    if(m[f] == 0) // 若字典未记录
                    {
                        m[f] = n;
                        //temp_set[n] = -1;// 则union[m1]=-1
                    }
                    else // 若字典已有某个集合
                    {
                        tag = true; //标记存在union[m]≠0
                        temp_set[n] = m[f]; // 则union[m1]=n
                        temp_set[m[f]] = -1;
                    }
                    n++;

                }

                if(tag != 0) // 存在union[m]≠0
                {
                    list<list<Rule>::iterator>::iterator it2 = leftEqual2i.begin();
                    Rule temp_rule;
                    for(int k = 1; k < n, it2 != leftEqual2i.end(); k++, it2++)
                    {
                        if(temp_set[k] == -1)
                        {
                            temp_set[k] = NonterminalLatestID;// 让根结点存储对应的新非终结符序号
                            AddVn(n_char + 1);// 分配新非终结符字符

                            int a = (*it2)->right[0];
                            (*it2)->right.erase((*it2)->right.begin());

                            // 新增文法C->B
                            temp_rule.left = temp_set[k];
                            if((*it2)->right.size() == 0)
                                temp_rule.right.push_back(value2key('#')); // epslion（ε）
                            else
                                temp_rule.right = (*it2)->right;
                            grammar.push_back(temp_rule);
                            // 把文法文法A->aB改为文法A->aC
                            (*it2)->right.clear();
                            (*it2)->right.push_back(a);
                            (*it2)->right.push_back(temp_set[k]);

                        }
                        else if(temp_set[k] != 0)
                        {
                            // 将文法A->aB修改为文法C->B
                            (*it2)->right.erase((*it2)->right.begin());//移除a
                            if((*it2)->right.size() == 0)
                                (*it2)->right.push_back(value2key('#'));//epslion
                            (*it2)->left = temp_set[temp_set[k]];// 把A改成C
                        }
                    }
                }
            }

            // 处理当前非终结符号
            i = (*it).left;
            leftEqual2i.clear();
            leftEqual2i.push_back(it);

        }
    }
    OrganizeGrammar();
    return PrintGrammar();
}

// 消除左递归
string GrammarProcessor::RemoveLeftRecursion()
{
    vector<vector<int>> rule_right;
    vector<int> temp_right;
    int NonterminalLatestID_max = NonterminalLatestID;
    for(int i = 0; i < NonterminalLatestID_max; i++) // 遍历每个非终结符号，设当前符号为Vni
    {
        if(v[i] != 0)
        {
            for(auto& g : grammar)
            {
                if(g.left == i) // 遍历以Vni为左部的文法
                {
                    temp_right.clear();
                    for(auto &j : g.right)
                    {
                        if(j < i)
                        {
                            // 将以Vnj为左部的文法代入到形式为Vni→αVnj β的文法中
                            for(auto &r : rule_right[j])
                                temp_right.push_back(r);
                        }
                        else
                        {
                            temp_right.push_back(j);
                        }
                    }
                    g.right = temp_right;
                }
            }

            bool tag = false;
            for(auto& g : grammar)
            {
                if(g.left == i && g.right[0] == i)//Vni存在形式为A->AX的文法
                {
                    tag = true;

                    AddVn(n_char + 1); // 新增非终结符B
                    // 将文法A->AX修改为文法B->XB
                    g.left = NonterminalLatestID - 1;
                    g.right.erase(g.right.begin());
                    g.right.push_back(NonterminalLatestID - 1);
                    // 新增文法B->ε
                    Rule temp_rule;
                    temp_rule.left = NonterminalLatestID - 1;
                    temp_rule.right.push_back(value2key('#'));
                    grammar.push_back(temp_rule);
                    break;
                }
            }

            if(tag == true)
            {
                tag = false;
                for(auto& g : grammar) // 遍历其它以Vni为左部的文法
                {
                    if(g.left == i)
                    {
                        g.right.push_back(NonterminalLatestID - 1); // 将B追加到最右部
                        if(tag == false)
                        {
                            tag = true;
                            rule_right.push_back(g.right); // rule_right[i]即以i为左部的规则的右部
                        }
                    }
                }
            }
            else
            {
                for(auto& g : grammar)
                    if(g.left == i)
                    {
                        rule_right.push_back(g.right); // rule_right[i]即以i为左部的规则的右部
                        break;
                    }
            }
        }

    }
    OrganizeGrammar();
    return PrintGrammar();
}
*/
string GrammarProcessor::PrintRule(const Rule& Rule)
{
    string ret;
    ret = ID2Word[Rule.Left];
    ret += " -> ";
    for(int RightID : Rule.Right)
    {
        ret += ID2Word[RightID] + " ";
    }
    ret += '\n';
    return ret;
}

string GrammarProcessor::PrintGrammar()
{
    string ret;
    for(auto& grammar : Grammar)
    {
        ret += PrintRule(grammar);
    }
    return ret;
}

int GrammarProcessor::Word2ID(string TargetWord)
{
    for(auto & pair : ID2Word)
    {
        if(pair.second == TargetWord)
            return pair.first;
    }
    cout << "---Error: Can not find ID from '" << TargetWord << "'!---" << endl;
    return -1;
}

bool GrammarProcessor::IsNonterminal(int num)
{
    return (num < 100 && num >= 0);
}

bool GrammarProcessor::IsTerminal(int num)
{
    return (num >= 100 && num < 200);
}
/*
// 整理字典
void GrammarProcessor::OrganizeDict()
{
    memset(temp_set, 0, sizeof(temp_set));
    for(auto & g : grammar)
    {
        temp_set[g.left] = 1;
        for(auto & r : g.right)
        {
            temp_set[r] = 1;
        }
    }

    int i, j;
    j = 0;
    map<int, char> temp_v;
    for(i = 0; i < NonterminalLatestID; i++)
    {
        if(temp_set[i] == 1)
        {
            temp_v[j] = v[i];
            j++;
        }
    }
    NonterminalLatestID = j;

    temp_v[100] = '$';
    temp_v[101] = '#';
    j = 102;
    for(i = 102; i < TerminalLatestID; i++)
    {
        if(temp_set[i] == 1)
        {
            temp_v[j] = v[i];
            j++;
        }
    }
    TerminalLatestID = j;

    v = temp_v;
}
*/
void GrammarProcessor::OrganizeGrammar()
{
    Grammar.sort();
}

