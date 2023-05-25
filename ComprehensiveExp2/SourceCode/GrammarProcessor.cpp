#include "GrammarProcessor.h"
#include "GrammarUtilities.h"

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
        while(GrammarUtil::IsSpace(*GrammarBuffer))
            GrammarBuffer++;

        // 读取左部
        TempWord = "";
        while(!GrammarUtil::IsSpace(*GrammarBuffer))
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
        while(GrammarUtil::IsSpace(*GrammarBuffer))
            GrammarBuffer++;
        if(GrammarBuffer[0]=='-' && GrammarBuffer[1]=='>')
            GrammarBuffer += 2;
        else
            return;
        while(GrammarUtil::IsSpace(*GrammarBuffer))
            GrammarBuffer++;

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
        if(*GrammarBuffer == '\r' || *GrammarBuffer == '\n')
            GrammarBuffer++;
    }
    OrganizeID2Word();
}

void GrammarProcessor::OrganizeID2Word()
{
    map<int, string> TempID2Word;
    NonterminalLatestID = 0;
    TempID2Word[100] = "epslion";
    TempID2Word[101] = "$";
    TerminalLatestID = 102;

    map<int, int> UpdateID;
    UpdateID[100] = 100;
    UpdateID[101] = 101;

    // 处理非终结符
    bool IsNonTerminal[200];
    memset(IsNonTerminal, false, sizeof(IsNonTerminal));
    for(const Rule& grammar : Grammar)
    {
        if(IsNonTerminal[grammar.Left] == false)
        {
            IsNonTerminal[grammar.Left] = true;

            TempID2Word[NonterminalLatestID] = ID2Word[grammar.Left];
            UpdateID[grammar.Left] = NonterminalLatestID;
            NonterminalLatestID++;
        }
    }

    // 处理终结符
    for(const Rule& grammar : Grammar)
        for(int Right : grammar.Right)
        {
            if(IsNonTerminal[Right] == false && UpdateID.find(Right) == UpdateID.end())
            {
                TempID2Word[TerminalLatestID] = ID2Word[Right];
                UpdateID[Right] = TerminalLatestID;
                TerminalLatestID++;
            }
        }

    // 替换
    ID2Word = TempID2Word;

    for(Rule& grammar : Grammar)
    {
        grammar.Left = UpdateID[grammar.Left];
        for(int& Right : grammar.Right)
            Right = UpdateID[Right];
    }
}

#pragma region "消除有害规则"{

void GrammarProcessor::RemoveHarmfulRules()
{
    list<Rule>::iterator it = Grammar.begin();
    int size = Grammar.size();
    for(int i = 0; i < size; i++)
    {
        if ((*it).Right.size() == 1 && (*it).Left == (*it).Right[0]) // U->U
            it = Grammar.erase(it);     // 删除有害的文法规则
        else
            it++;
    }
}

void GrammarProcessor::RemoveUnreachableRules()
{
    bool Reachable[200];
    memset(Reachable, 0, sizeof(Reachable));
    Reachable[0] = true; // 将开始符号放入集合中

    bool flag = true;
    while(flag)
    {
        flag = false;
        for (Rule grammar : Grammar)
        {
            if (!Reachable[grammar.Left])
                continue;

            for (int right : grammar.Right)
            {
                if(IsNonterminal(right) && Reachable[right] == false)
                {
                    Reachable[right] = true; // 放入“可到达集合”中
                    flag = true;
                }
            }
        }
    }

    list<Rule>::iterator it = Grammar.begin();
    for (int i = 0; i < Grammar.size(); i++)
    {
        if (Reachable[(*it).Left] == false)
            it = Grammar.erase(it); // 删除不可达的文法规则
        else
            it++;
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

void GrammarProcessor::RemoveUnterminableRules()
{
    memset(TempSet, -1, sizeof(TempSet)); // 意为Unterminable，是否可终止
    RemoveUnterminableRules_sub(0, 0);
    for(int ID = 0; ID <= NonterminalLatestID; ID++)
    {
        if(TempSet[ID] == 0)    // 如果该字符A被判为不可终止
        {
            list<Rule>::iterator it = Grammar.begin();
            for(int j = 0; j < Grammar.size(); j++) // 遍历所有文法
            {
                // 左部或右部含有该字符A,删除该不可终止的文法规则
                if((*it).Left == ID ||
                    find((*it).Right.begin(), (*it).Right.end(), ID) != (*it).Right.end())
                {
                    it = Grammar.erase(it);
                    continue;
                }

                it++;
            }
        }
    }
}

#pragma endregion }

string GrammarProcessor::SimplifyGrammar()
{
    RemoveHarmfulRules();
    //cout << "===Remove Harmful Rules===\n" << PrintGrammar() << '\n';
    RemoveUnreachableRules();
    //cout << "===Remove Unreachable Rules===\n" << PrintGrammar() << '\n';
    RemoveUnterminableRules();
    //cout << "===Remove Unterminable Rules===\n" << PrintGrammar() << '\n';
    OrganizeID2Word();
    SortGrammar();
    cout << "===Simplify Grammar Finally===\n" << PrintGrammar() << '\n';
    return PrintGrammar();
}

#pragma region "GetFirst(x)"{
void GrammarProcessor::GetFirst_sub(int x)
{
    for(const Rule& grammar : Grammar)
        if(grammar.Left == x)
        {
            int a = grammar.Right[0];
            if( IsTerminal(a) && find(first.begin(), first.end(), a)==first.end())
                    first.push_back(a);
            else if(a>0 && a!=x)//要求a!=x，避免死循环
                GetFirst_sub(a);
        }
}

list<int>& GrammarProcessor::GetFirst(int x)
{
    first.clear();
    if(IsNonterminal(x))
        GetFirst_sub(x);
    else
        first.push_back(x);
    first.sort();

    cout << "GetFirst: log: Get first Set of " << to_string(x) << " " <<  ID2Word[x] << '\n';
    cout.flush();

    return first;
}

string GrammarProcessor::GetFirst()
{
    string allFirst = "";
    vector<int> temp_vector;
    for(int i = 0; i <= NonterminalLatestID; i++)
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
    if(TempSet[x]!=0)// 避免死递归
        return {};

    TempSet[x] = 1;
    cout << "GetFollow: log: Getting Follow Set of " << to_string(x) << " " <<  ID2Word[x] << '\n';
    cout.flush();

    list<int> Follow;

    // ①当x是文法的开始符号，加入$
    if(x == 0)
    {
        Follow.push_back(Word2ID("$"));
        return Follow;
    }

    for(Rule grammar : Grammar)
    {
        if(x == 12)
            int i = 0;
        // ②当x是最右部的时候，加入$，
        // ④当x是最右部的时候，follow（X）包含follow（A），注意A≠X
        if(x == grammar.Right[ grammar.Right.size() - 1 ])
        {
            Follow.push_back(Word2ID("$"));

            if(grammar.Left != x)
                for(int id : GetFollow(grammar.Left))
                    Follow.push_back(id);

            continue;
        }

        bool tag = false;
        for(int right : grammar.Right)
        {
            if(right == x)                      // 假设A->αxβ，此时遍历到x
                tag = true;
            else if(tag == true)                // 此时已遍历到β
            {
                // ③存在一个产生式A->αxβ形式，那么follow（x）包含first（β）-ε
                list<int> first = GetFirst(right);
                for(int id1 : first)
                {
                    if(id1 != Word2ID("epslion"))
                    {
                        string sss = ID2Word[id1];
                        Follow.push_back(id1);
                    }

                    // ④如果first（β）包含ε，说明X可能是最后一个字符，那么follow（X）包含follow（A）
                    else if(grammar.Left != x)//避免死循环
                    {
                        if(x == 9)
                            int i = 0;
                        list<int> follow = GetFollow(grammar.Left);
                        for(int id2 : follow)
                            Follow.push_back(id2);
                    }
                }
                break;
            }
        }
    }

    set<int> DuplicatedFollow;// 去重
    for(int ID : Follow)
        DuplicatedFollow.insert(ID);
    Follow.clear();
    for(int ID : DuplicatedFollow)
        Follow.push_back(ID);

    return Follow;
}

string GrammarProcessor::GetFollow()
{
    string Ret = "";

    for(int i = 0; i <= NonterminalLatestID; i++)
    {
        Ret += "Follow(";
        Ret += ID2Word[i];
        Ret += ")={" ;
        memset(TempSet, 0, sizeof(TempSet));
        for(int ID : GetFollow(i))
        {
            Ret += ID2Word[ID];
            Ret += ", ";
        }
        Ret += "}\n";

        cout << "GetFollow: log: ===Finish get follow set of '" << ID2Word[i] << "'\ti=" << to_string(i) << "===\n\n";
        cout.flush();
    }
    return Ret;
}
#pragma endregion }

vector<Rule> GrammarProcessor::Substitute(const Rule& rule, int pos)
{
    int targetID = rule.Right[pos];
    if (!IsNonterminal(targetID))
    {
        cout << "LeftSubstitute: Error: can not substitute to " << PrintRule(rule) << ", pos = " << to_string(pos) << '\n';
        return {};
    }

    vector<Rule> Ret;
    for (const Rule& grammar : Grammar)
    {
        if (grammar.Left != targetID)
            continue;

        Rule temp(rule.Left, {});
        for (int right : rule.Right)
        {
            if (right == targetID)
                temp.Right.insert(temp.Right.end(), grammar.Right.begin(), grammar.Right.end());
            else
                temp.Right.push_back(right);
        }
        Ret.push_back(temp);
    }
    return Ret;
}

string GrammarProcessor::RemoveLeftCommonFactor()
{
    for(int nonterm = 0; nonterm <= NonterminalLatestID; nonterm++)// 遍历每个非终结符号，设当前符号为Vni
    {
        // 将左部为Vni的文法收纳到一起
        vector<list<Rule>::iterator> SameLeftRules = FindSameLeftRules(nonterm);
        if(SameLeftRules.size() <= 1)
            continue;

        // 求first集
        vector<list<int>> FirstContainer;
        for (auto rule : SameLeftRules)
            FirstContainer.push_back(GetFirst((*rule).Right[0]));

        // 遍历以Vni为左部的文法，找寻并消除间接左公因子
        bool ExistLCF = false;
        bool ExistIndrectLCF = true;
        while (ExistIndrectLCF)
        {
            ExistIndrectLCF = false;
            for (int i = 0; i < SameLeftRules.size(); i++)
                for (int j = i + 1; j < SameLeftRules.size(); j++)
                {
                    // first交集为空，非左公因子
                    const auto& f1 = FirstContainer[i];
                    const auto& f2 = FirstContainer[j];
                    list<int> inters;
                    set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), inserter(inters, inters.begin()));
                    if (inters.size() == 0)
                        continue;

                    // 若存在左公因子
                    ExistLCF = true;

                    // 直接左公因子
                    if (SameLeftRules[i]->Right[0] == SameLeftRules[j]->Right[0])
                        continue;

                    // 间接左公因子
                    ExistIndrectLCF = true;

                    auto subs1 = Substitute(*SameLeftRules[i], 0);
                    if (subs1.size() > 0)
                    {
                        *SameLeftRules[i] = subs1[0];//危
                        FirstContainer[i] = GetFirst(subs1[0].Right[0]);
                        for (int k = 1; k < subs1.size(); k++)
                        {
                            Grammar.push_back(subs1[k]);
                            SameLeftRules.push_back(prev(Grammar.end()));
                            FirstContainer.push_back(GetFirst(subs1[k].Right[0]));
                        }
                    }

                    auto subs2 = Substitute(*SameLeftRules[j], 0);
                    if (subs2.size() > 0)
                    {
                        *SameLeftRules[j] = subs2[0];//危
                        FirstContainer[j] = GetFirst(subs2[0].Right[0]);
                        for (int k = 1; k < subs2.size(); k++)
                        {
                            Grammar.push_back(subs2[k]);
                            SameLeftRules.push_back(prev(Grammar.end()));
                            FirstContainer.push_back(GetFirst(subs2[k].Right[0]));
                        }
                    }
                }
        }
        if(!ExistLCF)
            continue;

        // 建立并查集
        int Union[200];
        memset(Union, -1, sizeof(Union));//Union的值为-2代表这是父节点，-1代表不存在公因子，自然数代表该子节点的父节点的索引
        map<int, int> RightFirst2Index;
        for (int i = 0;i < SameLeftRules.size(); i++)
        {
            int First = SameLeftRules[i]->Right[0];// 取左因子
            if (RightFirst2Index.find(First) == RightFirst2Index.end()) // 若字典未记录
                RightFirst2Index[First] = i;
            else // 若字典已有某个集合
            {
                Union[i] = RightFirst2Index[First]; // 则Union[m1]=n
                Union[RightFirst2Index[First]] = -2;
            }
        }

        // 消除直接左公因子
        vector<list<Rule>::iterator>::iterator it2 = SameLeftRules.begin();
        for(int i = 0; i < SameLeftRules.size(), it2 != SameLeftRules.end(); i++, it2++)
        {
            if(Union[i] == -2)
            {
                AddNonterminal(ID2Word[(*it2)->Left]);// 分配新非终结符字符C
                Union[i] = NonterminalLatestID;// 让根结点存储对应的新非终结符序号

                int a = (*it2)->Right[0];//a
                (*it2)->Right.erase((*it2)->Right.begin());//B

                // 新增文法C->B
                Rule rule(Union[i], (*it2)->Right);
                if(rule.Right.size() == 0)
                    rule.Right.push_back(Word2ID("epslion")); // epslion（ε）
                Grammar.push_back(rule);
                // 把文法文法A->aB改为文法A->aC
                (*it2)->Right.clear();
                (*it2)->Right.push_back(a);
                (*it2)->Right.push_back(Union[i]);

            }
            else if(Union[i] != -1)
            {
                // 将文法A->aB修改为文法C->B
                (*it2)->Right.erase((*it2)->Right.begin());//移除a
                if((*it2)->Right.size() == 0)
                    (*it2)->Right.push_back(Word2ID("epslion"));//epslion
                (*it2)->Left = Union[Union[i]];// 把A改成C
            }
        }

        cout << "RemoveLeftCommonFactor: log: Finish remove left common factor, id=" << to_string(nonterm) << ", word=" << ID2Word[nonterm] << "\n";
        cout.flush();
    }

    SimplifyGrammar();

    return PrintGrammar();
}

vector<list<Rule>::iterator> GrammarProcessor::FindSameLeftRules(int Left)
{
    vector<list<Rule>::iterator> SameLeftRules;
    for(list<Rule>::iterator it = Grammar.begin(); it!=Grammar.end(); it++)
        if((*it).Left == Left)
            SameLeftRules.push_back(it);
    return SameLeftRules;
}

string GrammarProcessor::RemoveLeftRecursion()
{
    // 遍历每个非终结符号，设当前符号为Vni
    for(int i = 0; i <= NonterminalLatestID; i++)
    {
        bool existLR = false;
        for(Rule& grammar : Grammar)
        {
            if (grammar.Left != i || !IsNonterminal(grammar.Right[0]))
                continue;

            // 寻找所有的直接或间接左递归
            vector<int> rights = FindLeftRecur(i, grammar.Right[0]);
            if(rights.size() == 0)
                continue;

            // 消除左递归
            grammar.Right = rights;// 若存在左递归，则代入
            if(existLR == false)
            {
                existLR = true;

                // 新增非终结符A'
                AddNonterminal(ID2Word[grammar.Left]);

                // 将文法A->AX修改为文法A'->XA'
                grammar.Left = NonterminalLatestID;
                grammar.Right.erase(grammar.Right.begin());
                grammar.Right.push_back(NonterminalLatestID);

                // 新增文法A'->ε
                Rule temp_rule;
                temp_rule.Left = NonterminalLatestID;
                temp_rule.Right.push_back(Word2ID("epslion"));
                Grammar.push_back(temp_rule);
            }
            else
            {
                // 将文法A->AX修改为文法A'->XA'
                grammar.Left = NonterminalLatestID;
                grammar.Right.erase(grammar.Right.begin());
                grammar.Right.push_back(NonterminalLatestID);
            }
        }

        if (existLR == false)
            continue;

        for(Rule& grammar : Grammar) // 遍历所有以Vni为左部的文法
            if(grammar.Left == i)
            {
                if(grammar.Right[0] == Word2ID("epslion"))
                    grammar.Right.erase(grammar.Right.begin());
                grammar.Right.push_back(NonterminalLatestID); // 将A'追加到最右部
            }
        cout << "RemoveLeftRecursion: log: Finish remove left recursion, id=" << to_string(i) << ", word=" << ID2Word[i] << "\n";
        cout.flush();
    }

    SimplifyGrammar();

    return PrintGrammar();
}

vector<int> GrammarProcessor::FindLeftRecur(int TargetLeft, int CurrentLeft)
{
    for(const Rule& grammar : Grammar)
        if(grammar.Left == CurrentLeft)
        {
            if(grammar.Right[0] == TargetLeft)// 直接左递归
                return grammar.Right;
            else if(IsNonterminal(grammar.Right[0]) && grammar.Left!= grammar.Right[0])// 剪枝，并且避免死循环
            {
                vector<int> rights = FindLeftRecur(TargetLeft, grammar.Right[0]);
                if(rights.size() != 0)
                {
                    rights.insert(rights.begin(), grammar.Right.begin()+1, grammar.Right.end());// 间接左递归
                    return rights;
                }
            }
        }
    return {};
}

vector<vector<int>> GrammarProcessor::GetLL1Table()
{
    vector<vector<int>> LL1Table;
    Rule TempRule, EmptyRule;

    int Row = NonterminalLatestID + 2;
    int Col = TerminalLatestID - 100;

    vector<int> Cell;
    for(int i = 0; i < Row*Col; i++)
        LL1Table.push_back(Cell);

    // 第一格存储行数和列数
    LL1Table[0].push_back(Row);
    LL1Table[0].push_back(Col);

    // 第一行 每格 放一个终结符
    for(int i = 1; i < Col; i++)
        LL1Table[i].push_back(100 + i);//第i列（列数从0开始）放着ID为i+102的终结符

    // 第一列 每格 放一个非终结符
    for(int i = Col, NonterminalID = 0; i < Row*Col; i+=Col, NonterminalID++)
        LL1Table[i].push_back(NonterminalID);

    // 初始化表体
    for(Rule grammar : Grammar)
    {
        // 对于First(α)中的每个记号a，都将A->α添加到项目M[A,a]中
        for(int id : GetFirst(grammar.Right[0]))
        {
            int row = grammar.Left+1;
            // 若ε在First(a)中，则对于Follow(A) 的每个元素a（记号或是$），都将A->α添加到M[A,a]中
            if(id == 100)
            {
                for(int id2 : GetFollow(grammar.Left))
                {
                    int col = id2 - 102;
                    LL1Table[row * Col + col].push_back(grammar.Left);
                    for(int id : grammar.Right)
                        LL1Table[row * Col + col].push_back(id);
                }
            }
            else
            {
                int col = id - 100;
                LL1Table[row * Col + col].push_back(grammar.Left);
                for(int id : grammar.Right)
                    LL1Table[row * Col + col].push_back(id);
            }
        }
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

//    string Ret;
//    Ret = to_string(Rule.Left);
//    Ret += " -> ";
//    for(int RightID : Rule.Right)
//        Ret += to_string(RightID) + " ";
//    Ret += '\n';
//    return Ret;
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
    if(TargetWord == "epslion")
        return 100;

    for(auto & pair : ID2Word)
        if(pair.second == TargetWord)
            return pair.first;

    //cout << "Word2ID: warning: Can not find ID from '" << TargetWord << "'!\n";
    return -1;
}

void GrammarProcessor::AddNonterminal(string BaseSymbol)
{
    string NewSymbol = BaseSymbol + "'";
    while(Word2ID(NewSymbol) != -1)
        NewSymbol += "'";

    NonterminalLatestID++;
    ID2Word[NonterminalLatestID] = NewSymbol;
    //cout << "AddNonterminal: log: Add new non-terminal '" << NewSymbol << "' success!\n";
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

