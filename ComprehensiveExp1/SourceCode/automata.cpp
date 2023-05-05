#include "Automata.h"

#include <queue>
#include <map>
#include <algorithm>
#include <iostream>

void PrintTable(const vector<vector<int>>& FATable)
{
    if(FATable.size() > 200)
        return;

    int row = FATable[0][0];
    int col = FATable[0][1];
    string output = "";
    for(int i = 0; i < row; i++)
    {
        for(int j = 0; j < col; j++)
        {
            output += '|';
            for(int id : FATable[i * col + j])
            {
                if(i == 0 && j != 0)
                    output += id;
                else
                    output += to_string(id) + ",";
            }
            output += "\t\t";
        }
        output += '\n';
    }
    cout << output;
    cout.flush();

    int i = 0;
}

vector<vector<int>> Automata::GetDFA(const vector<vector<int>>& NFA)
{
    cout << "=== NFA ===" << endl;
    PrintTable(NFA);

    vector<vector<int>> DFA = GenerateDFA(NFA);
    cout << "=== DFA ===" << endl;
    PrintTable(DFA);

    vector<vector<int>> SimplifiedDFA = SimplifyDFA(DFA);
    cout << "=== SimplifiedDFA ===" << endl;
    PrintTable(SimplifiedDFA);

    return SimplifiedDFA;
}

/**
 * @brief Depth-first search for all ID2s that can be reached by ID1 through epslion.
 *        Note that make sure no epslion dead loops in the NFA.
 * @param NFA : The NFA Table
 * @param TableCell : The cell that store all ids
 * @param ID1 : The ID of StartNode.id from NFA
 */
void DFS(const vector<vector<int>>& NFA, vector<int>& TableCell, int ID1)
{
    TableCell.push_back(ID1);// StartNode

    int NFACol = NFA[0][1];

    for(int ID2 : NFA[ ID1 * NFACol + 1])// epslion
       DFS(NFA, TableCell, ID2);// OtherNode
}

vector<vector<int>> Automata::GenerateDFA(const vector<vector<int>>& NFA)
{
    vector<vector<int>> DFA;        // for Table
    vector<int> TableCell;

    queue<vector<int>> StateNode;   // for Graph
    vector<int> Latest;
    int LatestDFARowIndex;
    queue<char> q;

    // 建立{NFA图结点ID -> NFA表下标}映射表
    map<int, int> ID2Index;
    int NFARow = NFA[0][0];
    int NFACol = NFA[0][1];
    for(int i = NFACol; i < NFARow * NFACol; i += NFACol)
        ID2Index[NFA[i][0]] = i;

    // 初始化DFA表头
    int DFARow = 1;
    int DFACol = NFACol-1;
    DFA.push_back(TableCell);       //最后再填充第一格
    for(int i = 1; i < DFACol; i++) //第一行 照搬除了epslion之外的NFA的所有转移条件
    {
        TableCell.clear();
        TableCell.push_back(NFA[i+1][0]);
        DFA.push_back(TableCell);
    }

    // 从NFA的起始节点开始，把StartNode--ε-->OtherNode全都放进第二行第一列
    TableCell.clear();
    DFS(NFA, TableCell, NFA[0][2]);
    StateNode.push(TableCell);

    /*循环*/
    while (!StateNode.empty())
    {
        // 从预备队列中，取出可能的DFA图的新节点
        Latest = StateNode.front();
        StateNode.pop();

        // 检查DFA图中是否已存在这个节点
        bool HasExisted = false;
        for (int j = 1; j < DFARow; j++)
        {
            if (Latest == DFA[j * DFACol])
            {
                HasExisted = true;
                break;
            }
        }

        if (HasExisted == false)    // 若不存在,则生成该节点，继续后续的初始化操作
            DFA.push_back(Latest);
        else                        // 若存在，则跳过
            continue;


        // 初始化后续的条件转移后的单元格
        TableCell.clear();
        for (int i = 1; i < DFACol; i++)
            DFA.push_back(TableCell);
        LatestDFARowIndex = DFARow * DFACol;

        // 根据DFA的新结点  和  除了epslion以外的转移条件，查询NFA表，获取对应的NFA终点结点，生成DFA的下一结点【注意去重】
        for (int DFAID : Latest)
            for (int i = 1; i < DFACol; i++)
                for (int NFAID : NFA[ID2Index[DFAID] + i + 1]) // 因为没有epsilon转换，所以+1
                    DFA[LatestDFARowIndex + i].push_back(NFAID);

        for (int i = 1; i < DFACol; i++)//用集合去重
        {
            set<int> s(DFA[LatestDFARowIndex + i].begin(), DFA[LatestDFARowIndex + i].end());
            DFA[LatestDFARowIndex + i].assign(s.begin(), s.end());
        }

        // 遍历下一结点里的id，追加epsilon能到达的所有id【注意去重】
        for (int i = 1; i < DFACol; i++)
        {
            // 使用中间容器暂存再遍历，而不直接遍历，是因为单元格里将会填入新id，影响直接遍历的结果
            TableCell = DFA[LatestDFARowIndex + i];
            for(int ID : TableCell)
                DFS(NFA, DFA[LatestDFARowIndex + i], ID);
            set<int> s(DFA[LatestDFARowIndex + i].begin(), DFA[LatestDFARowIndex + i].end());
            DFA[LatestDFARowIndex + i].assign(s.begin(), s.end());
        }

        // 取新生成的结点，放入预备队列中
        for (int i = 1; i < DFACol; i++)
        {
            if(DFA[LatestDFARowIndex + i].size() > 0)
                StateNode.push(DFA[LatestDFARowIndex + i]);
        }

        DFARow++;
    }

    DFA[0].push_back(DFARow);
    DFA[0].push_back(DFACol);
    DFA[0].push_back(-1);
    DFA[0].push_back(NFA[0][3]);

    return DFA;
}

vector<vector<int>> Automata::SimplifyDFA(const vector<vector<int>>& DFA)
{
    vector<vector<int>> SimplifiedDFA = DFA;
    vector<int> TableCell;
    map<vector<int>, int> NodeToID;

    int LatestID = 1;
    int Row = DFA[0][0];
    int Col = DFA[0][1];
    const int EndID = DFA[0][3];

    SimplifiedDFA[0].pop_back();
    SimplifiedDFA[0].pop_back();
    SimplifiedDFA[0].push_back(1);

    for(int i = Col; i < Row * Col; i++)
    {
        if(DFA[i].size() == 0)
            continue;

        // 分配新ID
        if(NodeToID.find(DFA[i]) == NodeToID.end())
        {
            NodeToID[DFA[i]] = LatestID;

            // 如果这个DFA结点是终结点，则记录在表头
            if(find(DFA[i].begin(), DFA[i].end(), EndID) != DFA[i].end())
                SimplifiedDFA[0].push_back(LatestID);

            LatestID++;
        }

        // 改写
        SimplifiedDFA[i].clear();
        SimplifiedDFA[i].push_back(NodeToID[DFA[i]]);
    }

    return SimplifiedDFA;
}

/**
 * @brief If Target belong to StatusGroup[index], return index
 */
int FindSetIndex(const vector<set<int>>& StatusGroup, int Target)
{
    for(int i = 0; i < StatusGroup.size(); i++)
        for(int ID : StatusGroup[i])
            if(ID == Target)
                return i;

    cout << "Error: can not find '" << Target << "' in StatusGroup!" << endl;
    return -1;
}

vector<vector<int>> Automata::GetMinDFA(const vector<vector<int>>& SimplifiedDFA)
{
    vector<vector<int>> MinDFA;
    vector<int> TableCell;
    vector<set<int>> StatusGroup;
    vector<set<int>> TempStatusGroup;
    set<int> TempSet;

    // 获取基本信息
    int DFARow = SimplifiedDFA[0][0];
    int DFACol = SimplifiedDFA[0][1];
    int MinDFARow = 0;
    int MinDFACol = DFACol;
    vector<int> EndState;
    for(int i = 3; i < SimplifiedDFA[0].size(); i++)
        EndState.push_back(SimplifiedDFA[0][i]);

    map<int, int> Condition2ColIndex;
    for (int i = 1; i < DFACol; i++)
        Condition2ColIndex[SimplifiedDFA[i][0]] = i;

    map<int, int> ID2RowIndex;  // 请确保传入的DFA是简化后的DFA，否则DFA图节点没有唯一的ID
    for (int i = DFACol; i < DFARow * DFACol; i+=DFACol)
        ID2RowIndex[SimplifiedDFA[i][0]] = i;

    //对于DFA中的每个状态，分为接受状态组（包含NFA终态z）和非接受状态组（不包含z）
    for(int state : EndState)
        TempSet.insert(state);
    StatusGroup.push_back(TempSet);// 接受状态组StatusGroup[0]
    TempSet.clear();
    for(int i = DFACol; i < DFARow * DFACol; i++)
        for(int ID : SimplifiedDFA[i])
            if(find(EndState.begin(), EndState.end(), ID) == EndState.end())
                TempSet.insert(ID);
    StatusGroup.push_back(TempSet);// 非接受状态组StatusGroup[1]
    TempSet.clear();

    bool flag = true;//是否存在新的划分
    set<int>::iterator FrontID;
    while(flag)// 如果不存在新的划分时，终止循环
    {
        flag = false;
        for(set<int>& Group : StatusGroup)// 1.遍历每个状态集
        {
            // TempStatusGroup的大小设为StatusGroup.size()
            TempSet.clear();
            for(int i = 0; i < StatusGroup.size(); i++)
                TempStatusGroup.push_back(TempSet);

            // 遍历每个FrontID--Condition-->BackID
            for(auto element : Condition2ColIndex)// 2.遍历每个转移条件
            {
                int defaultBackID = -1; // -1代表不存在下一节点
                FrontID = Group.begin();
                while(FrontID != Group.end())// 3.遍历状态集里的每个id
                {
                    bool next = false;
                    for(int BackID : SimplifiedDFA[ID2RowIndex[*FrontID] + element.second])
                    {
                        // 初次赋值
                        if(defaultBackID == -1 && FrontID == Group.begin())
                        {
                            defaultBackID = BackID;
                            continue;
                        }

                        // 下一节点一样
                        if(BackID == defaultBackID)
                            continue;

                        // 下一节点不一样,并且与默认值所属的集合也不一样
                        if((defaultBackID == -1) ||
                            (FindSetIndex(StatusGroup, defaultBackID) != FindSetIndex(StatusGroup, BackID) ))
                        {
                            int SetIndex = FindSetIndex(StatusGroup, BackID);
                            if(SetIndex != -1)
                                TempStatusGroup[SetIndex].insert(*FrontID);

                            cout << "erase " << *FrontID << endl;
                            FrontID = Group.erase(FrontID); // erase之后，函数返回下一容器的头指针
                            next = true;

                            flag = true;// 标记，存在新的划分
                        }
                    }
                    if(next == false)
                        FrontID++;
                }

                // 将TempStatusGroup中的不为空的组并入旧组，清空TempStatusGroup
                if(flag)
                {
                    for(set<int> NewGroup : TempStatusGroup)
                        if(!NewGroup.empty())
                            StatusGroup.push_back(NewGroup);//此举将丢失当前Group的地址，因此借助flag回退，重新执行循环1.
                    break;
                }
            }
            TempStatusGroup.clear();
            if(flag)
                break;
        }
    }

    // TODO: comment多了状态4,5
    // 去除空集
    vector<set<int>>::iterator GroupIt = StatusGroup.begin();
    while(GroupIt != StatusGroup.end())
    {
        if(GroupIt->size() == 0)
            GroupIt = StatusGroup.erase(GroupIt);
        else
            GroupIt++;
    }

    // 填充MinDFA表格
    MinDFARow = StatusGroup.size() + 1;
    TableCell.clear();
    MinDFA.push_back(TableCell);    // 第一格
    MinDFA[0].push_back(MinDFARow);     // 记录行数
    MinDFA[0].push_back(MinDFACol);     // 记录列数
    MinDFA[0].push_back(FindSetIndex(StatusGroup, SimplifiedDFA[0][2]) + 1);             // 记录开始结点
    for(int i = 3; i < SimplifiedDFA[0].size(); i++)
    {
        int id = FindSetIndex(StatusGroup, SimplifiedDFA[0][i]) + 1;
        if(find(MinDFA[0].begin(), MinDFA[0].end(), id) == MinDFA[0].end())// 去重
            MinDFA[0].push_back(id);    // 记录终态结点
    }
    for (int i = 1; i < MinDFACol; i++) // 第一行（转移条件）与DFA一致
        MinDFA.push_back(SimplifiedDFA[i]);
    for (int i = MinDFACol; i < MinDFARow * MinDFACol; i++)
    {
        MinDFA.push_back(TableCell);
        if(i % MinDFACol == 0)
            MinDFA[i].push_back(i / MinDFACol);
    }

    for(int i = DFACol; i < DFARow * DFACol; i++)
    {
        int col = i % DFACol;
        if(col == 0 || SimplifiedDFA[i].size() == 0)
            continue;

        int id = FindSetIndex(StatusGroup, SimplifiedDFA[i][0]) + 1;
        int row = i / DFACol;
        row = SimplifiedDFA[row * DFACol][0];
        row = FindSetIndex(StatusGroup, row) + 1;

        int targetIndex = row * MinDFACol + col;
        if(find(MinDFA[targetIndex].begin(), MinDFA[targetIndex].end(), id) == MinDFA[targetIndex].end())
            MinDFA[row * MinDFACol + col].push_back(id);
    }

    cout << "=== MinDFA ===" << endl;
    PrintTable(MinDFA);
    return MinDFA;
}
