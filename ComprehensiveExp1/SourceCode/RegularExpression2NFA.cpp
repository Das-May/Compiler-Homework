#include "RegularExpression2NFA.h"

#include <stack>
#include <queue>
#include <set>
#include <algorithm>
#include <iostream>

vector<vector<int>> RegularExpression2NFA::GetNFA(string Regex)
{
    GenerateNFA(Regex);
    return NFATable;
}

void RegularExpression2NFA::GenerateNFA(string Regex)
{
	maxId = 0;
	totalCondition.clear();
	totalCondition.push_back('#');

	stack<NfaChunk> s;
	string temp1 = "", temp2 = "";

	NfaChunk tempChunk, opChunk, tempChunk_2;
    for(auto& c : Regex)
	{
		if (isalpha(c))
		{
			s.push(And(c));
            if (count(totalCondition.begin(), totalCondition.end(), c) == 0)// 如果容器中还记录过这个转移条件
				totalCondition.push_back(c);//记录
		}
		else 
		{
			switch (c)
			{
			case '*':
				tempChunk = Expand_zero(s.top());
				s.pop();
				break;
			case '+':
				tempChunk = Expand_one(s.top());
				s.pop();
				break;
			case '?':
				tempChunk = Choose(s.top());
				s.pop();
				break;
			case ')':
				tempChunk = s.top();
				s.pop();

				while (s.top().op != '(')
				{
					while (s.top().op == '#')
					{
						tempChunk = Connect(s.top(), tempChunk);
						s.pop();
					}

					if (s.top().op == '|')
					{
						opChunk = s.top();
						s.pop();

						tempChunk_2 = s.top();
						s.pop();
						while (s.top().op == '#')
						{
							tempChunk_2 = Connect(s.top(), tempChunk_2);
							s.pop();
						}
						tempChunk = Or(tempChunk, tempChunk_2);
					}					
				}
				s.pop(); // 弹出'('
			default:
				tempChunk.op = c;
				break;
			}
			s.push(tempChunk);
		}
	}

    NFAGraph = s.top();
	s.pop();
	while (!s.empty())
	{
        NFAGraph = Connect(s.top(), NFAGraph);
		s.pop();
	}
}

#pragma region Operator Functions{

NfaChunk RegularExpression2NFA::And(char c)
{
	// 单个字符，会产生三个NFA结点
	NfaChunk andChunk;
	NfaNode* midNode = new NfaNode();

	andChunk.start->id = maxId;					// 分配id
	midNode->id = maxId + 1;
	andChunk.end->id = maxId + 2;
	maxId += 3;

	andChunk.start->nextNode.push_back(midNode);	//连接
	andChunk.start->transition[midNode] = '#';
	midNode->nextNode.push_back(andChunk.end);
	midNode->transition[andChunk.end] = c;

	return andChunk;
}

NfaChunk RegularExpression2NFA::Or(NfaChunk a, NfaChunk b)
{
	// 创建新起点，并连接a块的起点
	NfaNode* newStartNode = new NfaNode();
	newStartNode->id = maxId;
	maxId++;
	newStartNode->nextNode.push_back(a.start);
	newStartNode->transition[a.start] = '#';
	//以a块的起点未分支点，连接b块的第二个节点
	a.start->nextNode.push_back(b.start->nextNode[0]);
	a.start->transition[b.start->nextNode[0]] = '#';
	delete b.start;
	// 创建新终点,并将a块和b块的终点汇集过去
	NfaNode* newEndNode = new NfaNode();
	newEndNode->id = maxId;
	maxId++;
	a.end->nextNode.push_back(newEndNode);
	a.end->transition[newEndNode] = '#';
	b.end->nextNode.push_back(newEndNode);
	b.end->transition[newEndNode] = '#';
	// 返回新块
	a.start = newStartNode;
	a.end = newEndNode;
	return a;
}

NfaChunk RegularExpression2NFA::Expand_zero(NfaChunk a)
{
	// *
	// 回头绕,转移条件为epsilon
	NfaNode* midNode = a.start->nextNode[0];
	a.end->nextNode.push_back(midNode);
	a.end->transition[midNode] = '#';
	// 往后大绕,转移条件为epsilon
	NfaNode* newEndNode = new NfaNode();
	newEndNode->id = maxId;
	maxId++;
	a.end->nextNode.push_back(newEndNode);
	a.end->transition[newEndNode] = '#';
	a.start->nextNode.push_back(newEndNode);
	a.start->transition[newEndNode] = '#';
	// 返回新块
	a.end = newEndNode;
	return a;
}

NfaChunk RegularExpression2NFA::Expand_one(NfaChunk a)
{
	// +

	return Connect(a, Expand_zero(a));//错的
}

NfaChunk RegularExpression2NFA::Choose(NfaChunk a)
{
	// ?
	// 直接在首部和尾部之间加一个ε连线
	a.start->nextNode.push_back(a.end);
	a.start->transition[a.end] = '#';
	return a;
}

NfaChunk RegularExpression2NFA::Connect(NfaChunk head, NfaChunk tail)
{
	head.end->nextNode = tail.start->nextNode;// 连接
	head.end->transition = tail.start->transition;
	delete tail.start;
	head.end = tail.end;						// 更新
	return head;
}
#pragma endregion}

void RegularExpression2NFA::ChangeNFAGraphToTable()
{
	// 初始化表格容器
    NFATable.clear();
	int col = totalCondition.size();
	int row = maxId;
	int i, j;
	vector<int> temp;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
            NFATable.push_back(temp);
		}
	}

    // 初始化遍历标记
    mark = new bool* [maxId];
    for (i = 0; i < maxId; i++)
    {
        mark[i] = new bool[maxId];
        for (j = 0; j < maxId; j++)
            mark[i][j] = false;
    }
	
    DFSNFA(NFAGraph.start);
}

void RegularExpression2NFA::DFSNFA(NfaNode* root)
{
    if (root == NFAGraph.end)
		return;

	for (auto& nextNode : root->nextNode)
	{
		if (mark[root->id][nextNode->id] == false)
		{
			mark[root->id][nextNode->id] = true;
			int i = 0;
			for (auto& condition : totalCondition)
			{
				if (condition == root->transition[nextNode])
				{
                    NFATable[root->id * totalCondition.size() + i].push_back(nextNode->id);
				}
				i++;
			}
            DFSNFA(nextNode);
		}
	}
}

vector<vector<int>> RegularExpression2NFA::GetDFA()
{
    DFATable.clear();
	int col = totalCondition.size();
    int currentId;//当前的id值，同时也是NFATable的行索引号
    int currentRow = -1;//当前DFATable的行索引号
	int i = 0;
	vector<int> temp, latest;
	queue<int> q;
	queue<vector<int>> colQueue;

	// 初始化
    // 从NFAGraph.start开始，把epsilon能到达的所有id放进DFA容器里，此为第一列的第一个DFA图结点
    currentId = NFAGraph.start->id;
	temp.push_back(currentId);
	q.push(currentId);

	while (!q.empty())
	{
		currentId = q.front();
		q.pop();
        for (auto& id : NFATable[currentId * col])
		{
			temp.push_back(id);
			q.push(id);
		}
	}
	colQueue.push(temp);

	/*循环*/
	while (!colQueue.empty())
	{
		latest = colQueue.front();
		colQueue.pop();

        DFATable.push_back(latest);
		currentRow++;

		// 初始化后续的条件转移后的id容器
		temp.clear();
		for (i = 1; i < col; i++)
		{
            DFATable.push_back(temp);
		}

		// 遍历DFA第一列的最新容器的id，获取对应的NFA条件列的id，追加到DFA条件列【注意去重】
		for (auto& id : latest)
		{
			for (i = 1; i < col; i++)
			{
                for (auto id2 : NFATable[id * col + i])
				{
                    DFATable[currentRow * col + i].push_back(id2);
				}
			}

		}
		for (i = 1; i < col; i++)
		{
            set<int> s(DFATable[currentRow * col + i].begin(), DFATable[currentRow * col + i].end());
            DFATable[currentRow * col + i].assign(s.begin(), s.end());
		}

		// 遍历条件列的id，追加epsilon能到达的所有id【注意去重】
		for (i = 1; i < col; i++)
		{
			while (!q.empty())// 清空队列
				q.pop();

            for (auto& id : DFATable[currentRow * col + i])//遍历条件列的id
				q.push(id);

			while (!q.empty())//追加epsilon能到达的所有id
			{
				currentId = q.front();
				q.pop();

                for (auto id : NFATable[currentId * col])//epsilon列
				{
                    if (find(DFATable[currentRow * col + i].begin(), DFATable[currentRow * col + i].end(), id)
                        == DFATable[currentRow * col + i].end())// 去重：容器中还没有这个id
					{
                        DFATable[currentRow * col + i].push_back(id);//则追加id
						q.push(id);
					}
					
				}
			}
		}
			

		// 取条件列的容器，若不为空，且若此前第一列中没有相等的列，则放进第一列
		for (i = 1; i < col; i++)
		{
			bool tag = true;
			for (int j = 0; j < currentRow; j++)
			{
                if (DFATable[currentRow * col + i] == DFATable[j * col])
				{
					tag = false;
					break;
				}
			}

            if (tag && DFATable[currentRow * col + i].size() != 0)
                colQueue.push(DFATable[currentRow * col + i]);
		}

		// 若为空，或第一列中已存在相等列，则不放进
	}

    return DFATable;
}

vector<vector<int>> RegularExpression2NFA::GetMinDFA()
{
	// 重新整理
	int currentId = 1;
	int i;
	int col = totalCondition.size();

	vector<vector<int>>::iterator it, it_find;
	map<vector<int>, int> m;
	map<int, int> map_setId;
	vector<int> temp;
    MinDFATable.clear();

	// 初步划分非终态idSet[] = 0 和 终态idSet[] = 1
    int endId = NFAGraph.end->id;

	int tag  = 0;	// 判断非终态

    for (it = DFATable.begin(); it != DFATable.end(); it++)		// 遍历DFA表里的每个集合
	{
		if (find((*it).begin(), (*it).end(), endId) != (*it).end())	// 若该集合包含NFA	的终点节点
		{
			tag = 1;	// 则为终态
		}

		if ((*it).size() != 0)
		{
			// 为每个集合重命名id
            it_find = find(DFATable.begin(), it, *it);
			if (it_find == it)	// 未命名
			{
                SimpleDFATable.push_back(currentId);
				m[*it] = currentId;
				currentId++;

				// 划分子集（非终态集0，和，终态集1）
				map_setId[m[*it]] = tag;
			}
			else				// 已命名
			{
                SimpleDFATable.push_back(m[*it]);
			}	
		}
		else // 空集用0表示
		{
            SimpleDFATable.push_back(0);
		}
	}
	vector<string> idSet;
	string s = "";
	i = 0;
    for (auto& t : SimpleDFATable)
	{
		
		
		s += map_setId[t] + '0';
		i++;
		if (i % col == 0 && i != 0)
		{
			idSet.push_back(s);
			s = "";
		}
		
	}

	// 初始化最小dfa
	temp.clear();
    for (auto& id : SimpleDFATable)
        MinDFATable.push_back(temp);

	// 合并
    int row = SimpleDFATable.size() / col;
	tag = 0;
	for (i = 0; i < row; i++)
	{
		tag = 0;
		for (int j = 0; j < i; j++)
		{
			if (idSet[i] == idSet[j])
			{
				tag = 1;
				Combine(j, i);
				break;
			}
		}
		if (tag == 0)
		{
			Combine(i, i);
		}
	}
	
	cout << "\n化简后的DFA表格式:" << endl;
	for (auto& condition : totalCondition)
		cout << condition << '\t';
	i = 0;
    for (auto& id : SimpleDFATable)
	{
		if (i % col == 0)
		{
			cout << '\n';
		}
		i++;
		cout << id << '\t';
	}

    return MinDFATable;
}

void RegularExpression2NFA::Combine(int srcRow, int destRow)
{
	int col = totalCondition.size();
	for (int i = 0; i < col; i++)
	{
        MinDFATable[srcRow * col + i].push_back(SimpleDFATable[destRow * col + i]);
	}
}
