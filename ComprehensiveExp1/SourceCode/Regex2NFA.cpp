#include "Regex2NFA.h"

#include <stack>
#include <algorithm>
#include <iostream>

vector<vector<int>> Regex2NFA::GetNFA(string Regex)
{
    GenerateNFA(Regex);
    ChangeNFAGraphToTable();
    return NFATable;
}

void Regex2NFA::GenerateNFA(string Regex)
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

NfaChunk Regex2NFA::And(char c)
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

NfaChunk Regex2NFA::Or(NfaChunk a, NfaChunk b)
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

NfaChunk Regex2NFA::Expand_zero(NfaChunk a)
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

NfaChunk Regex2NFA::Expand_one(NfaChunk a)
{
	// +

	return Connect(a, Expand_zero(a));//错的
}

NfaChunk Regex2NFA::Choose(NfaChunk a)
{
	// ?
	// 直接在首部和尾部之间加一个ε连线
	a.start->nextNode.push_back(a.end);
	a.start->transition[a.end] = '#';
	return a;
}

NfaChunk Regex2NFA::Connect(NfaChunk head, NfaChunk tail)
{
	head.end->nextNode = tail.start->nextNode;// 连接
	head.end->transition = tail.start->transition;
	delete tail.start;
	head.end = tail.end;						// 更新
	return head;
}
#pragma endregion}

void Regex2NFA::ChangeNFAGraphToTable()
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

void Regex2NFA::DFSNFA(NfaNode* root)
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
