#include "GrammarProcessor.h"

#include <algorithm>
#include <set>
#include <iostream>

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
            TempRule.Left = ID;

        // 跳过->
        bool end = false;
        while(*GrammarBuffer == ' ' || *GrammarBuffer == '-' || *GrammarBuffer == '>')
        {
            if(*GrammarBuffer == '>')
                end = true;
            else if(end && (*GrammarBuffer == '-' || *GrammarBuffer == '>'))//防止吞并文法符号，例如-，--，-=,>,>=
                break;
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
                TempRule.Right.push_back(ID);

            // 跳过空格和间隔
            while(*GrammarBuffer == ' ' || *GrammarBuffer == '|' || *GrammarBuffer == '\r')
                GrammarBuffer++;
        }

        Grammar.push_back(TempRule);

        // 跳过换行符
        if(*GrammarBuffer == '\r' && *GrammarBuffer == '\n')
            GrammarBuffer++;
    }
    OrganizeID2Word();
}

void GrammarProcessor::OrganizeID2Word()
{
    bool IsNonTerminal[200];
    memset(IsNonTerminal, false, sizeof(IsNonTerminal));
    for(Rule& grammar : Grammar)
        IsNonTerminal[grammar.Left] = true;

    map<int, string> TempID2Word;
    NonterminalLatestID = 0;
    TempID2Word[100] = "epslion";
    TempID2Word[101] = "$";
    TerminalLatestID = 102;

    map<int, int> UpdateID;

    for(Rule& grammar : Grammar)
    {
        if(UpdateID.find(grammar.Left) == UpdateID.end())
        {
            TempID2Word[NonterminalLatestID] = ID2Word[grammar.Left];
            UpdateID[grammar.Left] = NonterminalLatestID;
            NonterminalLatestID++;
        }

        for(int Right : grammar.Right)
        {
            if(UpdateID.find(Right) == UpdateID.end())
            {
                if(IsNonTerminal[Right] == true)
                {
                    TempID2Word[NonterminalLatestID] = ID2Word[Right];
                    UpdateID[Right] = NonterminalLatestID;
                    NonterminalLatestID++;
                }
                else
                {
                    TempID2Word[TerminalLatestID] = ID2Word[Right];
                    UpdateID[Right] = TerminalLatestID;
                    TerminalLatestID++;
                }
            }
        }
    }

    ID2Word = TempID2Word;

    for(auto& grammar : Grammar)
    {
        grammar.Left = UpdateID[grammar.Left];
        for(int& Right : grammar.Right)
            Right = UpdateID[Right];
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
            it = Grammar.erase(it);     // 删除有害的文法规则
            it--;
        }
    }
}

void GrammarProcessor::RemoveUnreachableRules()
{
    bool Reachable[200];
    memset(Reachable, 0, sizeof(Reachable));
    Reachable[0] = true; // 将左部开始符号放入集合中
    for(Rule grammar : Grammar)
        if(Reachable[grammar.Left] == true) // 若左部在集合中
            for(auto& right : grammar.Right)
                if(IsNonterminal(right)) // 对于右部的每个非终结符号
                    Reachable[right] = true; // 放入“可到达集合”中

    list<Rule>::iterator it = Grammar.begin();
    int size = Grammar.size();
    for(int i = 0; i < size; i++, it++)
    {
        if(Reachable[(*it).Left] == false)
        {
            it = Grammar.erase(it); // 删除不可达的文法规则
            it--;
        }
    }
}

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
                            break;
                    }
                }
            }
        }
        else if(Processed == true) //（剪枝操作）
            break;
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
                    it = Grammar.erase(it);// 删除该不可终止的文法规则
                    it--;
                }
                else
                {
                    for(int RightID : (*it).Right)
                    {
                        if(RightID == ID)  // 右部含有该字符A
                        {
                            it = Grammar.erase(it);// 删除该不可终止的文法规则
                            it--;
                            break;
                        }
                    }
                }
            }
        }
    }
}

#pragma endregion }

string GrammarProcessor::SimplifyGrammar()
{
    RemoveHarmfulRules();
    RemoveUnreachableRules();
    RemoveUnterminableRules();
    OrganizeID2Word();
    return PrintGrammar();
}

#pragma region "GetFirst(x)"{
void GrammarProcessor::GetFirst_sub(int x)
{
    bool tag = false;
    for(auto& g : Grammar)
    {
        if(g.Left == x)    // 遍历以X为左部的每个文法
        {
            tag = true;
            int a = g.Right[0]; // 取文法右部第一个字符假设为a
            if( IsTerminal(a) )     // 若a是终结符或ε
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
                GetFirst_sub(a);    // 则递归进入GetFirst(int a)

        }
        else if(tag == true) //（剪枝操作）
            break;
    }
}

list<int>& GrammarProcessor::GetFirst(int x)
{
    first.clear();
    if(IsNonterminal(x))
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
        if(ID2Word.find(i) != ID2Word.end())
        {
            allFirst += "First(";
            allFirst += ID2Word[i];
            allFirst += ")={" ;
            temp_vector.clear();
            for(auto& element : GetFirst(i))
            {
                if(find(temp_vector.begin(), temp_vector.end(), element) == temp_vector.end())
                    temp_vector.push_back(element);
            }
            for(auto& element : temp_vector)
            {
                allFirst += ID2Word[element];
                allFirst += ", ";
            }
            allFirst += "}\n";
        }
    }
    return allFirst;
}
#pragma endregion }

#pragma region "GetFollow(x)"{
list<int> GrammarProcessor::GetFollow(int x)
{
    list<int> Follow;

    if(x == 0) // ①当x是文法的开始符号
    {
        Follow.push_back(Word2ID("$")); // 加入$
    }

    for(auto& grammar : Grammar) // 遍历每个文法
    {
        if(x == grammar.Right[ grammar.Right.size() - 1 ])   // ②当x是最右部的时候
        {
            Follow.push_back(Word2ID("$")); // 加入$

            // ④如果存在一个产生式A→αB，那么follow（B）包含follow（A），注意A≠B
            if(grammar.Left != x)
            {
                for(auto& f : GetFollow(grammar.Left))
                {
                    Follow.push_back(f);
                }
            }
        }

        bool tag = false;
        for(auto& r : grammar.Right)
        {
            if(r == x)
                tag = true;
            else if(tag == true)// ③此时已遍历到β，即存在一个产生式A→αBβ
            {
                list<int> kkk = GetFirst(r);
                for(auto& f : kkk)
                {
                    if(f != Word2ID("#"))    // 那么follow（B）包含first（β）-ε
                        Follow.push_back(f);
                    else if(grammar.Left != x)    // ④存在产生式A→αBβ且first（β）包含ε，注意A≠B
                        for(auto& f : GetFollow(grammar.Left)) // 那么follow（B）包含follow（A）
                            Follow.push_back(f);
                }
            }
        }
    }

    return Follow;
}

string GrammarProcessor::GetFollow()
{
    string Ret = "";
    set<int> DuplicatedFollow;
    for(int i = 0; i < NonterminalLatestID; i++)
    {
        if(ID2Word.find(i) != ID2Word.end())
        {
            Ret += "First(";
            Ret += ID2Word[i];
            Ret += ")={" ;
            DuplicatedFollow.clear();
            for(int ID : GetFollow(i))
                DuplicatedFollow.insert(ID);

            for(auto& ID : DuplicatedFollow)
            {
                Ret += ID2Word[ID];
                Ret += ", ";
            }
            Ret += "}\n";
        }
    }
    return Ret;
}
#pragma endregion }

string GrammarProcessor::RemoveLeftCommonFactor()
{
    list<list<Rule>::iterator> SameLeftRules;
    int Union[200];
    int i = 0;
    map<list<int>, int> RightFirstSet2Index;
    int Index = 0;

    SortGrammar();// 排序文法规则，确保相同左部的文法是相邻的

    list<Rule>::iterator it = Grammar.begin();
    for(; it != Grammar.end(); it++)// 遍历每个非终结符号，设当前符号为Vni
    {
        if((*it).Left == i)    // 遍历以Vni为左部的每个文法
            SameLeftRules.push_back(it); // 装入容器list<Rule>中
        else    // 左部i改变了，说明上一个遍历完了
        {
            // 处理上一个非终结符号
            if(SameLeftRules.size() != 1) // 若个数≠1
            {
                RightFirstSet2Index.clear();
                memset(Union, 0, sizeof (Union) );
                Index = 1;// !!!
                int tag = false;

                for(auto & rule : SameLeftRules ) // 遍历以Vni为左部的文法
                {
                    int Right = (*rule).Right[0]; // 右部第一个字符
                    list<int> First = GetFirst(Right);
                    if(RightFirstSet2Index.find(First) == RightFirstSet2Index.end()) // 若字典未记录
                    {
                        RightFirstSet2Index[First] = Index;
                        //Union[n] = -1;// 则Union[m1]=-1
                    }
                    else // 若字典已有某个集合
                    {
                        tag = true; //标记存在Union[m]≠0
                        Union[Index] = RightFirstSet2Index[First]; // 则Union[m1]=n
                        Union[RightFirstSet2Index[First]] = -1;
                    }
                    Index++;

                }

                if(tag != 0) // 存在union[m]≠0
                {
                    list<list<Rule>::iterator>::iterator it2 = SameLeftRules.begin();
                    Rule TempRule;
                    for(int k = 1; k < Index, it2 != SameLeftRules.end(); k++, it2++)
                    {
                        if(Union[k] == -1)
                        {
                            Union[k] = NonterminalLatestID;// 让根结点存储对应的新非终结符序号
                            AddNonterminal(ID2Word[(*it2)->Left]);// 分配新非终结符字符

                            int a = (*it2)->Right[0];
                            (*it2)->Right.erase((*it2)->Right.begin());

                            // 新增文法C->B
                            TempRule.Left = Union[k];
                            if((*it2)->Right.size() == 0)
                                TempRule.Right.push_back(Word2ID("#")); // epslion（ε）
                            else
                                TempRule.Right = (*it2)->Right;
                            Grammar.push_back(TempRule);
                            // 把文法文法A->aB改为文法A->aC
                            (*it2)->Right.clear();
                            (*it2)->Right.push_back(a);
                            (*it2)->Right.push_back(Union[k]);

                        }
                        else if(Union[k] != 0)
                        {
                            // 将文法A->aB修改为文法C->B
                            (*it2)->Right.erase((*it2)->Right.begin());//移除a
                            if((*it2)->Right.size() == 0)
                                (*it2)->Right.push_back(Word2ID("#"));//epslion
                            (*it2)->Left = Union[Union[k]];// 把A改成C
                        }
                    }
                }
            }

            // 处理当前非终结符号
            i = (*it).Left;
            SameLeftRules.clear();
            SameLeftRules.push_back(it);

        }
    }

    SortGrammar();

    return PrintGrammar();
}

string GrammarProcessor::RemoveLeftRecursion()
{
    // ID2Right[i]即以i为左部的规则的右部
    // 构造这个表的条件是，必须确保先消除左公因子
    vector<vector<int>> ID2Right;
    vector<int> TempRight;
    int MaxNonterminalID = NonterminalLatestID;
    for(int i = 0; i < MaxNonterminalID; i++) // 遍历每个非终结符号，设当前符号为Vni
    {
        if(ID2Word.find(i) != ID2Word.end())
        {
            for(auto& grammar : Grammar)
            {
                if(grammar.Left == i) // 遍历以Vni为左部的文法
                {
                    TempRight.clear();
                    for(int j : grammar.Right)
                    {
                        // 将以Vnj为左部的文法代入到形式为Vni -> α Vnj β的文法中
                        if(j < i)
                            for(int ID : ID2Right[j])
                                TempRight.push_back(ID);
                        else
                            TempRight.push_back(j);
                    }
                    grammar.Right = TempRight;
                }
            }

            bool IsExistedLeftRecursion = false;
            for(auto& grammar : Grammar)
            {
                if(grammar.Left == i && grammar.Right[0] == i)//Vni存在形式为A->AX的文法
                {
                    IsExistedLeftRecursion = true;

                    // 新增非终结符A'
                    AddNonterminal(ID2Word[grammar.Left]);

                    // 将文法A->AX修改为文法A'->XA'
                    grammar.Left = NonterminalLatestID - 1;
                    grammar.Right.erase(grammar.Right.begin());
                    grammar.Right.push_back(NonterminalLatestID - 1);

                    // 新增文法A'->ε
                    Rule temp_rule;
                    temp_rule.Left = NonterminalLatestID - 1;
                    temp_rule.Right.push_back(Word2ID("#"));
                    Grammar.push_back(temp_rule);
                    break;
                }
            }

            if(IsExistedLeftRecursion == true)
            {
                IsExistedLeftRecursion = false;
                for(auto& grammar : Grammar) // 遍历其它以Vni为左部的文法
                {
                    if(grammar.Left == i)
                    {
                        grammar.Right.push_back(NonterminalLatestID - 1); // 将A'追加到最右部
                        if(IsExistedLeftRecursion == false)
                        {
                            IsExistedLeftRecursion = true;
                            ID2Right.push_back(grammar.Right);
                        }
                    }
                }
            }
            else
            {
                for(auto& grammar : Grammar)
                {
                    if(grammar.Left == i)
                    {
                        ID2Right.push_back(grammar.Right);
                        break;
                    }
                }
            }
        }
    }

    SortGrammar();

    return PrintGrammar();
}

vector<vector<int>> GrammarProcessor::GetLL1Table()
{
    vector<vector<int>> LL1Table;
    Rule TempRule, EmptyRule;
    vector<int> Cell;

    OrganizeID2Word();
    SortGrammar();
    int Row = NonterminalLatestID + 1;
    int Col = TerminalLatestID - 102 + 1;

    // 初始化表头
    Cell.push_back(Row);
    Cell.push_back(Col);
    LL1Table.push_back(Cell);

    for(int i = 0; i < (Col-1); i++)
    {
        Cell.clear();
        Cell.push_back(102 + i);
        LL1Table.push_back(Cell);
    }

    // 初始化表体
    for(int Index = Col, NonterminalID = 0; Index < Row*Col, NonterminalID < NonterminalLatestID; Index++)
    {
        if(Index % Col == 0)
        {
            // 回填上一行
            if(NonterminalID-1 > 0)
            {
                bool tag = false;
                for(Rule& grammar : Grammar)
                {
                    if(grammar.Left == NonterminalID-1)
                    {
                        tag = true;

                        for(int Terminal : GetFirst(grammar.Right[0]))
                        {
                            int row = NonterminalID;
                            int col = Terminal - 102 + 1;
                            for(int Right : grammar.Right)
                                LL1Table[row * Col + col].push_back(Right);
                        }

                    }
                    else if(tag == true)
                        break;
                }
            }

            // 填写第一列的非终结符
            Cell.clear();
            Cell.push_back(NonterminalID);
            NonterminalID++;
            LL1Table.push_back(Cell);

            Cell.clear();
        }
        // 先把其它列填入空vector<int>
        else
            LL1Table.push_back(Cell);
    }

    return LL1Table;
}

string GrammarProcessor::PrintRule(const Rule& Rule)
{
    string Ret;
    Ret = ID2Word[Rule.Left];
    Ret += " -> ";
    for(int RightID : Rule.Right)
        Ret += ID2Word[RightID] + " ";
    Ret += '\n';
    return Ret;
}

string GrammarProcessor::PrintGrammar()
{
    string Ret;
    for(auto& grammar : Grammar)
        Ret += PrintRule(grammar);
    return Ret;
}

int GrammarProcessor::Word2ID(string TargetWord)
{
    for(auto & pair : ID2Word)
        if(pair.second == TargetWord)
            return pair.first;

    cout << "Warning: Can not find ID from '" << TargetWord << "'!" << endl;
    return -1;
}

void GrammarProcessor::AddNonterminal(string BaseSymbol)
{
    string NewSymbol = BaseSymbol + "'";
    if(Word2ID(NewSymbol) == -1)
    {
        ID2Word[NonterminalLatestID] = NewSymbol;
        NonterminalLatestID++;
    }
    else
        cout << "Add new non-terminal '" << NewSymbol << "' failed!" << endl;
}

bool GrammarProcessor::IsNonterminal(int num)
{
    return (num < 100 && num >= 0);
}

bool GrammarProcessor::IsTerminal(int num)
{
    return (num >= 100 && num < 200);
}

void GrammarProcessor::SortGrammar()
{
    Grammar.sort();
}

map<int, string> GrammarProcessor::GetID2Word()
{
    return ID2Word;
}

