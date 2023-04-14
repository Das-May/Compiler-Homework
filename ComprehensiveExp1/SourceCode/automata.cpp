#include "Automata.h"

#include <queue>
#include <map>
#include <algorithm>
#include <iostream>

vector<vector<int>> Automata::GetDFA(const vector<vector<int>>& NFA)
{
    vector<vector<int>> DFA = GenerateDFA(NFA);

    SimplifyDFA(DFA);

    return DFA;
}

vector<vector<int>> Automata::GetMinDFA(const vector<vector<int>>& DFA)
{
    vector<vector<int>> MinDFA;
    vector<int> TableCell;
    vector<set<int>> StatusGroup;
    vector<set<int>> TempStatusGroup;
    set<int> NewSet;

    int DFARow = DFA[0][0];
    int DFACol = DFA[0][1];
    int MinDFARow = 0;
    int MinDFACol = DFACol;
    vector<int> EndState;
    for(int i = 2; i < DFA[0].size(); i++)
    {
        EndState.push_back(DFA[0][i]);
    }
    map<int, int> Condition2ColIndex;
    for (int i = 1; i < DFACol; i++)
    {
        Condition2ColIndex[DFA[i][0]] = i;
    }
    map<int, int> ID2RowIndex;
    for (int i = DFACol; i < DFARow * DFACol; i+=DFACol)
    {
        Condition2ColIndex[DFA[i][0]] = i;
    }

    // 初始化
    TableCell.clear();
    MinDFA.push_back(TableCell);
    for (int i = 1; i < MinDFACol; i++)
        MinDFA.push_back(DFA[i]);

    //对于DFA中的每个状态，分为接受状态组（包含NFA终态z）和非接受状态组（不包含z）
    for(int state : EndState)
    {
        NewSet.insert(state);
    }
    StatusGroup.push_back(NewSet);// 接受状态组StatusGroup[0]
    NewSet.clear();
    for(vector<int> cell : DFA)
    {
        if(find(EndState.begin(), EndState.end(), cell[0]) == EndState.end())
        {
            NewSet.insert(cell[0]);
        }
    }
    StatusGroup.push_back(NewSet);// 非接受状态组StatusGroup[1]
    NewSet.clear();

    bool flag = true;
    set<int>::iterator FrontID;
    while(flag)
    {
        flag = false;
        for(set<int> Group : StatusGroup)
        {
            // TempStatusGroup的大小设为Group.size()
            for(int i = 0; i < StatusGroup.size(); i++)
            {
                TempStatusGroup.push_back(NewSet);
            }

            // 遍历每个FrontID--Condition-->BackID
            for(FrontID = Group.begin(); FrontID != Group.end(); FrontID++)
            {
                for(auto element : Condition2ColIndex)
                {
                    for(int BackID : DFA[ID2RowIndex[*FrontID] * DFACol + element.second])
                    {
                        // BackID与FrontID不同集合
                        if(Group.find(BackID) == Group.end())
                        {
                            flag = true;    // 标记，存在新的划分

                            int SetIndex = FindSetIndex(StatusGroup, BackID);
                            if(SetIndex != -1)
                            {
                                TempStatusGroup[SetIndex].insert(*FrontID);
                            }
                            else
                            {
                                cout << "出事儿了";
                            }

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
                MinDFA[LatestID * MinDFACol + i].push_back(DFA[ID2RowIndex[ID] * DFACol + i][0]);
            }
        }
    }

    return MinDFA;
}

vector<vector<int>> Automata::GenerateDFA(const vector<vector<int>>& NFA)
{
    vector<vector<int>> DFA;
    vector<int> TableCell;
    queue<vector<int>> StateNode;
    vector<int> Latest;
    int LatestDFARowIndex;
    queue<char> q;
    char currentId;

    // 建立{NFA图结点ID -> NFA表下标}映射表
    map<char, int> ID2Index;
    int NFARow = NFA[0][0];
    int NFACol = NFA[0][1];
    for(int i = NFACol; i < NFARow * NFACol; i += NFACol)
    {
        ID2Index[NFA[i][0]] = i;
    }

    // 初始化DFA表头（照搬除了ε之外的NFA的所有转移条件）
    int DFACol = NFA[0][1] - 1;
    int DFARow = 1;
    DFA.push_back(TableCell);
    for(int i = 1; i < DFACol; i++)
    {
        TableCell.clear();
        TableCell.push_back(NFA[i+1][0]);
        DFA.push_back(TableCell);
    }

    // 从NFA的起始节点开始，把StartNode--ε-->OtherNode全都放进第二行第一列
    TableCell.clear();
    TableCell.push_back(NFA[DFACol +1][0]);
    for(auto &i : NFA[DFACol + 2])
        TableCell.push_back(i);
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
        {
            DFA.push_back(TableCell);
        }
        LatestDFARowIndex = DFARow * DFACol;

        // 根据DFA的新结点  和  除了epslion以外的转移条件，查询NFA表，获取对应的NFA终点结点，生成DFA的下一结点【注意去重】
        for (auto& DFAID : Latest)
        {
            for (int i = 1; i < DFACol; i++)
            {
                for (auto NFAID : NFA[ID2Index[DFAID] + i + 1]) // 因为没有epsilon转换，所以+1
                {
                    DFA[LatestDFARowIndex + i].push_back(NFAID);
                }
            }

        }
        for (int i = 1; i < DFACol; i++)//用集合去重
        {
            set<int> s(DFA[LatestDFARowIndex + i].begin(), DFA[LatestDFARowIndex + i].end());
            DFA[LatestDFARowIndex + i].assign(s.begin(), s.end());
        }

        // 遍历下一结点里的id，追加epsilon能到达的所有id【注意去重】
        for (int i = 1; i < DFACol; i++)
        {
            while (!q.empty())// 清空队列
                q.pop();

            for (auto& id : DFA[LatestDFARowIndex + i])//遍历条件列的id
                q.push(id);

            while (!q.empty())//追加epsilon能到达的所有id
            {
                currentId = q.front();
                q.pop();

                for (auto id : NFA[ ID2Index[currentId] + 1 ])//epsilon列
                {

                    if (find(DFA[LatestDFARowIndex + i].begin(), DFA[LatestDFARowIndex + i].end(), id) != DFA[LatestDFARowIndex + i].end())// 去重：容器中还没有这个id
                    {
                        DFA[LatestDFARowIndex + i].push_back(id);//则追加id
                        q.push(id);
                    }

                }
            }
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
    DFA[0].push_back(NFA[ (NFARow - 1) * NFACol][0]);

    return DFA;
}

void Automata::SimplifyDFA(vector<vector<int>>& DFA)
{
    int LatestID = 1;
    int Row = DFA[0][0];
    int Col = DFA[0][1];
    const int EndID = DFA[0][2];
    DFA[0].pop_back();
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
    {
        for(int ID : StatusGroup[i])
        {
            if(ID == Target)
            {
                return i;
            }
        }
    }
    return -1;
}
