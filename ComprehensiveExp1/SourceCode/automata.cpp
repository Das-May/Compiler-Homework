#include "Automata.h"

#include <queue>
#include <map>
#include <algorithm>
#include <iostream>

vector<vector<int>> Automata::GetDFA(const vector<vector<int>>& NFA)
{
    cout << "=== NFA ===" << endl;
    PrintTable(NFA);

    vector<vector<int>> DFA = GenerateDFA(NFA);
    cout << "=== DFA ===" << endl;
    PrintTable(DFA);

    SimplifyDFA(DFA);
    cout << "=== SimplifiedDFA ===" << endl;
    PrintTable(DFA);

    return DFA;
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
//    int currentId;

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
    // TODO:epslion能到达更深的层次
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

void Automata::SimplifyDFA(vector<vector<int>>& DFA)
{
    int LatestID = 1;
    int Row = DFA[0][0];
    int Col = DFA[0][1];
    const int EndID = DFA[0][3];
    DFA[0].pop_back();
    DFA[0].pop_back();
    DFA[0].push_back(1);// 起始节点的ID为1
    map<vector<int>, int> NodeToID;

    for(int i = Col; i < Row * Col; i++)
    {
        if(NodeToID.find(DFA[i]) == NodeToID.end())
        {
            NodeToID[DFA[i]] = LatestID;

            // 如果这个DFA结点是终结点，则记录在表头
            for(int ID : DFA[i])
            {
                if(ID == EndID)
                {
                    DFA[0].push_back(LatestID);
                    break;
                }
            }

            LatestID++;
        }

        DFA[i].clear();
        DFA[i].push_back(NodeToID[DFA[i]]);
    }
}

int Automata::FindSetIndex(const vector<set<int>>& StatusGroup, int Target)
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
    for(int i = 2; i < SimplifiedDFA[0].size(); i++)
        EndState.push_back(SimplifiedDFA[0][i]);

    map<int, int> Condition2ColIndex;
    for (int i = 1; i < DFACol; i++)
        Condition2ColIndex[SimplifiedDFA[i][0]] = i;

    map<int, int> ID2RowIndex;  // 请确保传入的DFA是简化后的DFA，否则DFA图节点没有唯一的ID
    for (int i = DFACol; i < DFARow * DFACol; i+=DFACol)
        Condition2ColIndex[SimplifiedDFA[i][0]] = i;

    // 初始化
    TableCell.clear();
    MinDFA.push_back(TableCell);
    for (int i = 1; i < MinDFACol; i++) // 第一行（转移条件）与DFA一致
        MinDFA.push_back(SimplifiedDFA[i]);

    //对于DFA中的每个状态，分为接受状态组（包含NFA终态z）和非接受状态组（不包含z）
    for(int state : EndState)
        TempSet.insert(state);
    StatusGroup.push_back(TempSet);// 接受状态组StatusGroup[0]
    TempSet.clear();
    for(vector<int> cell : SimplifiedDFA)
        if(find(EndState.begin(), EndState.end(), cell[0]) == EndState.end())
            TempSet.insert(cell[0]);
    StatusGroup.push_back(TempSet);// 非接受状态组StatusGroup[1]
    TempSet.clear();

    bool flag = true;
    set<int>::iterator FrontID;
    while(flag)
    {
        flag = false;
        for(set<int> Group : StatusGroup)
        {
            // TempStatusGroup的大小设为Group.size()
            for(int i = 0; i < StatusGroup.size(); i++)
                TempStatusGroup.push_back(TempSet);

            // 遍历每个FrontID--Condition-->BackID
            for(FrontID = Group.begin(); FrontID != Group.end(); FrontID++)
            {
                for(auto element : Condition2ColIndex)
                {
                    for(int BackID : SimplifiedDFA[ID2RowIndex[*FrontID] + element.second])
                    {
                        // BackID与FrontID不同集合
                        if(Group.find(BackID) == Group.end())
                        {
                            flag = true;    // 标记，存在新的划分

                            int SetIndex = FindSetIndex(StatusGroup, BackID);
                            if(SetIndex != -1)
                                TempStatusGroup[SetIndex].insert(*FrontID);

                            cout << "erase " << *FrontID << endl;
                            FrontID = Group.erase(FrontID); // erase之后，函数返回下一容器的头指针
                            FrontID--;                      // 与for循环里的++相抵消
                        }
                    }

                }
            }

            // 将NewGroup中的不为空的组并入旧组，清空NewGroup
            for(set<int> NewGroup : TempStatusGroup)
            {
                if(!NewGroup.empty())
                {
                    StatusGroup.push_back(NewGroup);
                }
            }
            TempStatusGroup.clear();
        }
    }

    // 合并
    MinDFARow = StatusGroup.size();
    int LatestID = 1;
    for(set<int> Group : StatusGroup)
    {
        TableCell.clear();
        for (int i = 0; i < MinDFACol; i++)
        {
            MinDFA.push_back(TableCell);
        }

        for(int ID : Group)
        {
            MinDFA[LatestID * MinDFACol].push_back(ID);
            for (int i = 1; i < MinDFACol; i++)
            {
                MinDFA[LatestID * MinDFACol + i].push_back(SimplifiedDFA[ID2RowIndex[ID] * DFACol + i][0]);
            }
        }
    }

    MinDFA[0].push_back(MinDFARow);
    MinDFA[0].push_back(MinDFACol);
    return MinDFA;
}

void Automata::PrintTable(const vector<vector<int>>& FATable)
{
    int row = FATable[0][0];
    int col = FATable[0][1];
    string output = "";
    for(int i = 0; i < row; i++)
    {
        for(int j = 0; j < col; j++)
        {
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
