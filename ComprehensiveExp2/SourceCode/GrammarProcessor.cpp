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

    cout << "Get first Set of " << to_string(x) << " " <<  ID2Word[x] << '\n';
    cout.flush();

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
    cout << "Getting Follow Set of " << to_string(x) << " " <<  ID2Word[x] << '\n';
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

    for(int i = 0; i < NonterminalLatestID; i++)
    {
        Ret += "Follow(";
        Ret += ID2Word[i];
        Ret += ")={" ;
        for(int ID : GetFollow(i))
        {
            Ret += ID2Word[ID];
            Ret += ", ";
        }
        Ret += "}\n";

        cout << "===Finish get follow set of '" << ID2Word[i] << "'\ti=" << to_string(i) << "===\n\n";
        cout.flush();
    }
    return Ret;
}
#pragma endregion }

string GrammarProcessor::RemoveLeftCommonFactor()
{
    list<list<Rule>::iterator> SameLeftRules;
    int Union[200];
    int Index = 0;

    for(int i = 0; i < NonterminalLatestID; i++)// 遍历每个非终结符号，设当前符号为Vni
    {
        SameLeftRules.clear();// 将左部为Vni的文法收纳到一起
        for(list<Rule>::iterator it = Grammar.begin(); it!=Grammar.end(); it++)
            if((*it).Left == i)
                SameLeftRules.push_back(it);

        if(SameLeftRules.size() != 1) // 若个数≠1
        {
            map<int, int> RightFirst2Index;
            memset(Union, 0, sizeof (Union) );
            Index = 1;// !!!
            int tag = false;

            for(auto & rule : SameLeftRules ) // 遍历以Vni为左部的文法
            {
                int First = (*rule).Right[0];// 取左因子

                if(RightFirst2Index.find(First) == RightFirst2Index.end()) // 若字典未记录
                    RightFirst2Index[First] = Index;
                else // 若字典已有某个集合
                {
                    tag = true; //标记存在Union[m]≠0
                    Union[Index] = RightFirst2Index[First]; // 则Union[m1]=n
                    Union[RightFirst2Index[First]] = -1;
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
                        AddNonterminal(ID2Word[(*it2)->Left]);// 分配新非终结符字符C

                        int a = (*it2)->Right[0];
                        (*it2)->Right.erase((*it2)->Right.begin());

                        // 新增文法C->B
                        TempRule.Left = Union[k];
                        if((*it2)->Right.size() == 0)
                            TempRule.Right.push_back(Word2ID("epslion")); // epslion（ε）
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
                            (*it2)->Right.push_back(Word2ID("epslion"));//epslion
                        (*it2)->Left = Union[Union[k]];// 把A改成C
                    }
                }
            }
        }

        cout << "Finish remove left common factor, id=" << to_string(i) << ", word=" << ID2Word[i] << "\n";
        cout.flush();
    }

    SortGrammar();

    return PrintGrammar();
}

string GrammarProcessor::RemoveLeftRecursion()
{
    SortGrammar();

    for(int i = 0; i < NonterminalLatestID; i++) // 遍历每个非终结符号，设当前符号为Vni
    {
        if(FindLeftRecur(i, i).size() == 0)
            continue;

        bool existLR = false;
        vector<int> rights;
        for(Rule& grammar : Grammar)
        {
            if(grammar.Left == i && IsNonterminal(grammar.Right[0]))// 寻找所有的直接或间接左递归
            {
                rights = FindLeftRecur(i, grammar.Right[0]);
                if(rights.size() == 0)// 若不存在左递归，结束对该文法的处理
                    continue;

                // 若存在左递归，则代入
                grammar.Right = rights;

                // 消除左递归
                if(existLR == false)
                {
                    existLR = true;

                    // 新增非终结符A'
                    AddNonterminal(ID2Word[grammar.Left]);

                    // 将文法A->AX修改为文法A'->XA'
                    grammar.Left = NonterminalLatestID - 1;
                    grammar.Right.erase(grammar.Right.begin());
                    grammar.Right.push_back(NonterminalLatestID - 1);

                    // 新增文法A'->ε
                    Rule temp_rule;
                    temp_rule.Left = NonterminalLatestID - 1;
                    temp_rule.Right.push_back(Word2ID("epslion"));
                    Grammar.push_back(temp_rule);
                }
                else
                {
                    // 将文法A->AX修改为文法A'->XA'
                    grammar.Left = NonterminalLatestID - 1;
                    grammar.Right.erase(grammar.Right.begin());
                    grammar.Right.push_back(NonterminalLatestID - 1);
                }
            }
        }

        if(existLR == true)
            for(Rule& grammar : Grammar) // 遍历所有以Vni为左部的文法
                if(grammar.Left == i)
                    grammar.Right.push_back(NonterminalLatestID - 1); // 将A'追加到最右部

        cout << "Finish remove left recursion, id=" << to_string(i) << ", word=" << ID2Word[i] << "\n";
        cout.flush();
    }

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
                    rights.insert(rights.end(), grammar.Right.begin()+1, grammar.Right.end());// 间接左递归
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

    OrganizeID2Word();
    SortGrammar();
    int Row = NonterminalLatestID + 1;
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

    cout << "Warning: Can not find ID from '" << TargetWord << "'!\n";
    return -1;
}

void GrammarProcessor::AddNonterminal(string BaseSymbol)
{
    string NewSymbol = BaseSymbol + "'";
    while(Word2ID(NewSymbol) != -1)
        NewSymbol += "'";

    ID2Word[NonterminalLatestID] = NewSymbol;
    NonterminalLatestID++;
    cout << "Add new non-terminal '" << NewSymbol << "' success!\n";
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

