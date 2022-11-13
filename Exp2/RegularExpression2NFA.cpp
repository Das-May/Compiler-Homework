#include "RegularExpression2NFA.h"


string RegularExpression2NFA::GetNFA(string RegularExpression)
{
	maxId = 0;
	totalCondition.clear();
	totalCondition.push_back('#');

	stack<NfaChunk> s;
	string temp1 = "", temp2 = "";

	NfaChunk tempChunk, opChunk, tempChunk_2;
	for(auto& c : RegularExpression)
	{
		if (isalpha(c))
		{
			s.push(And(c));
			if (std::count(totalCondition.begin(), totalCondition.end(), c) == 0)// 如果容器中还记录过这个转移条件
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

	Nfa_Graph = s.top();
	s.pop();
	while (!s.empty())
	{
		Nfa_Graph = Connect(s.top(), Nfa_Graph);
		s.pop();
	}

	PrintNfaGraph();
	return PrintNfaTable();
}

#pragma region 运算符函数
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
#pragma endregion

void RegularExpression2NFA::PrintNfaGraph()
{
	cout << "NFA图格式:" << endl;
	mark = new bool*[maxId];
	for (int i = 0; i < maxId; i++)
	{
		mark[i] = new bool[maxId];
	}
	for (int i = 0; i < maxId; i++)
	{
		for (int j = 0; j < maxId; j++)
		{
			mark[i][j] = false;
		}
	}

	DFSPrint(Nfa_Graph.start);
}

void RegularExpression2NFA::DFSPrint(NfaNode* root)
{
	if (root == Nfa_Graph.end)
		return;

	for (auto& nextNode : root->nextNode)
	{
		if (mark[root->id][nextNode->id] == false)
		{
			mark[root->id][nextNode->id] = true;
			cout << root->id << "---" << root->transition[nextNode] << "--->" << nextNode->id << endl;
			DFSPrint(nextNode);
		}
			
	}
		

}

string RegularExpression2NFA::PrintNfaTable()
{
	// 初始化表格容器
	int col = totalCondition.size();
	int row = maxId;
	int i, j;
	vector<int> temp;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			Nfa_Table.push_back(temp);
		}
	}

	/*
	// 试打印
	cout << '\t';
	for (auto& condition : totalCondition)
		cout << condition << '\t';
	j = 0;
	for (auto& id_set : Nfa_Table)
	{
		if (j % col == 0)
		{
			cout << '\n' << j / col << '\t';
		}
		for (auto& id : id_set)
		{
			cout << id << ',';
		}
		cout << '\t';

		j++;
	}*/
	
	string s = "\n---NFA表格式:---\n";
	//cout << "\nNFA表格式:" << endl;
	mark = new bool* [maxId];
	for (i = 0; i < maxId; i++)
	{
		mark[i] = new bool[maxId];
		for (j = 0; j < maxId; j++)
			mark[i][j] = false;
	}
	DFSPrint_table(Nfa_Graph.start);

	s += '\t';
	for (auto& condition : totalCondition)
		s = s + condition + '\t';
	j = 0;
	for (auto& id_set : Nfa_Table)
	{
		if (j % col == 0)
		{
			s = s + '\n' + to_string(j / col) + '\t';
		}
		for (auto& id : id_set)
		{
			s = s + to_string(id) + ',';
		}
		s += '\t';

		j++;
	}
	s += '\n';
	return s;
}

void RegularExpression2NFA::DFSPrint_table(NfaNode* root)
{
	if (root == Nfa_Graph.end)
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
					Nfa_Table[root->id * totalCondition.size() + i].push_back(nextNode->id);
				}
				i++;
			}
			DFSPrint_table(nextNode);
		}

	}


}

string RegularExpression2NFA::GetDFA()
{
	int col = totalCondition.size();
	int currentId;//当前的id值，同时也是Nfa_Table的行索引号
	int currentRow = -1;//当前Dfa_Table的行索引号
	int i = 0;
	vector<int> temp, latest;
	queue<int> q;
	queue<vector<int>> colQueue;

	// 初始化
	// 从Nfa_Graph.start开始，把epsilon能到达的所有id放进DFA容器里，此为第一列的第一个DFA图结点
	currentId = Nfa_Graph.start->id;
	temp.push_back(currentId);
	q.push(currentId);

	while (!q.empty())
	{
		currentId = q.front();
		q.pop();
		for (auto& id : Nfa_Table[currentId * col])
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

		Dfa_Table.push_back(latest);
		currentRow++;

		// 初始化后续的条件转移后的id容器
		temp.clear();
		for (i = 1; i < col; i++)
		{
			Dfa_Table.push_back(temp);
		}

		// 遍历DFA第一列的最新容器的id，获取对应的NFA条件列的id，追加到DFA条件列【注意去重】
		for (auto& id : latest)
		{
			for (i = 1; i < col; i++)
			{
				for (auto id2 : Nfa_Table[id * col + i])
				{
					Dfa_Table[currentRow * col + i].push_back(id2);
				}
			}

		}
		for (i = 1; i < col; i++)
		{
			set<int> s(Dfa_Table[currentRow * col + i].begin(), Dfa_Table[currentRow * col + i].end());
			Dfa_Table[currentRow * col + i].assign(s.begin(), s.end());
		}

		// 遍历条件列的id，追加epsilon能到达的所有id【注意去重】
		for (i = 1; i < col; i++)
		{
			while (!q.empty())// 清空队列
				q.pop();

			for (auto& id : Dfa_Table[currentRow * col + i])//遍历条件列的id
				q.push(id);

			while (!q.empty())//追加epsilon能到达的所有id
			{
				currentId = q.front();
				q.pop();

				for (auto id : Nfa_Table[currentId * col])//epsilon列
				{
					if (find(Dfa_Table[currentRow * col + i].begin(), Dfa_Table[currentRow * col + i].end(), id)
						== Dfa_Table[currentRow * col + i].end())// 去重：容器中还没有这个id
					{
						Dfa_Table[currentRow * col + i].push_back(id);//则追加id
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
				if (Dfa_Table[currentRow * col + i] == Dfa_Table[j * col])
				{
					tag = false;
					break;
				}
			}

			if (tag && Dfa_Table[currentRow * col + i].size() != 0)
				colQueue.push(Dfa_Table[currentRow * col + i]);
		}

		// 若为空，或第一列中已存在相等列，则不放进
	}

	/*输出*/
	string s = "\n---DFA表格式:---\n";
	for (auto& condition : totalCondition)
		s = s + condition + '\t';
	i = 0;
	for (auto& id_set : Dfa_Table)
	{
		if (i % col == 0)
			s += '\n';

		for (auto& id : id_set)
		{
			s = s + to_string(id) + ',';
		}
		s += '\t';
		
		i++;
	}
	s += '\n';
	return s;
}

string RegularExpression2NFA::GetMinDFA()
{
	// 重新整理
	int currentId = 1;
	int i;
	int col = totalCondition.size();

	vector<vector<int>>::iterator it, it_find;
	map<vector<int>, int> m;
	map<int, int> map_setId;
	vector<int> temp;


	// 初步划分非终态idSet[] = 0 和 终态idSet[] = 1
	int endId = Nfa_Graph.end->id;

	int tag  = 0;	// 判断非终态

	for (it = Dfa_Table.begin(); it != Dfa_Table.end(); it++)		// 遍历DFA表里的每个集合
	{
		if (find((*it).begin(), (*it).end(), endId) != (*it).end())	// 若该集合包含NFA	的终点节点
		{
			tag = 1;	// 则为终态
		}

		if ((*it).size() != 0)
		{
			// 为每个集合重命名id
			it_find = find(Dfa_Table.begin(), it, *it);
			if (it_find == it)	// 未命名
			{
				simpleDfa_Table.push_back(currentId);
				m[*it] = currentId;
				currentId++;

				// 划分子集（非终态集0，和，终态集1）
				map_setId[m[*it]] = tag;
			}
			else				// 已命名
			{
				simpleDfa_Table.push_back(m[*it]);
			}	
		}
		else // 空集用0表示
		{
			simpleDfa_Table.push_back(0);
		}
	}
	vector<string> idSet;
	string s = "";
	i = 0;
	for (auto& t : simpleDfa_Table)
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
	for (auto& id : simpleDfa_Table)
		minDfa_Table.push_back(temp);

	// 合并
	int row = simpleDfa_Table.size() / col;
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
	for (auto& id : simpleDfa_Table)
	{
		if (i % col == 0)
		{
			cout << '\n';
		}
		i++;
		cout << id << '\t';
	}


	/*输出*/
	s =  "\n---最小DFA表格式:---\n";
	for (auto& condition : totalCondition)
		s = s + condition + '\t';
	i = 0;
	for (auto& set : minDfa_Table)
	{
		if (i % col == 0)
		{
			s += '\n';
		}
		i++;
		for (auto& id : set)
			s = s + to_string(id) + ',';
		s += '\t';
	}
	s += '\n';
	return s;
}

void RegularExpression2NFA::Combine(int srcRow, int destRow)
{
	int col = totalCondition.size();
	for (int i = 0; i < col; i++)
	{
		minDfa_Table[srcRow * col + i].push_back(simpleDfa_Table[destRow * col + i]);
	}
}
