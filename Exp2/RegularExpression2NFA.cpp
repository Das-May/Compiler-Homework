#include "RegularExpression2NFA.h"


void RegularExpression2NFA::GetNFA(string RegularExpression)
{
	currentId = 0;
	totalCondition.clear();
	totalCondition.push_back(' ');
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

	Nfa = s.top();
	s.pop();
	while (!s.empty())
	{
		Nfa = Connect(s.top(), Nfa);
		s.pop();
	}

	PrintNfaGraph();
	PrintNfaTable();
}

NfaChunk RegularExpression2NFA::And(char c)
{
	// 单个字符，会产生三个NFA结点
	NfaChunk andChunk;
	NfaNode* midNode = new NfaNode();

	andChunk.start->id	= currentId;					// 分配id
	midNode		  ->id	= currentId + 1;
	andChunk.end  ->id	= currentId + 2;
	currentId += 3;
		
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
	newStartNode->id = currentId;
	currentId++;
	newStartNode->nextNode.push_back(a.start);
	newStartNode->transition[a.start] = '#';
	//以a块的起点未分支点，连接b块的第二个节点
	a.start->nextNode.push_back(b.start->nextNode[0]);
	a.start->transition[b.start->nextNode[0]] = '#';
	delete b.start;
	// 创建新终点,并将a块和b块的终点汇集过去
	NfaNode* newEndNode = new NfaNode();
	newEndNode->id = currentId;
	currentId++;
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
	newEndNode->id		= currentId;
	currentId++;
	a.end->nextNode.push_back(newEndNode);
	a.end->transition[newEndNode] = '#';
	// 返回新块
	a.end = newEndNode;
	return a;
}

NfaChunk RegularExpression2NFA::Expand_one(NfaChunk a)
{
	// +

	return Connect(a, Expand_zero(a));//错的
}

NfaChunk RegularExpression2NFA::Connect(NfaChunk head, NfaChunk tail)
{
	head.end->nextNode   = tail.start->nextNode;// 连接
	head.end->transition = tail.start->transition;
	delete tail.start;
	head.end = tail.end;						// 更新
	return head;
}

void RegularExpression2NFA::PrintNfaGraph()
{
	cout << "NFA图格式:" << endl;
	mark = new bool*[currentId];
	for (int i = 0; i < currentId; i++)
	{
		mark[i] = new bool[currentId];
	}
	for (int i = 0; i < currentId; i++)
	{
		for (int j = 0; j < currentId; j++)
		{
			mark[i][j] = false;
		}
	}

	DFSPrint(Nfa.start);
}

void RegularExpression2NFA::DFSPrint(NfaNode* root)
{
	if (root == Nfa.end)
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

void RegularExpression2NFA::PrintNfaTable()
{
	// 初始化表格容器
	int col = totalCondition.size();
	int row = currentId;
	int i, j;
	for (i = 1; i < row; i++)
	{
		totalCondition.push_back(i);
		for (j = 1; j < col; j++)
		{
			totalCondition.push_back(-1);
		}
	}

	j = 0;
	for (auto& id : totalCondition)
	{
		if(j >= col)
			cout << id << '\t';
		else
			cout << char(id) << '\t';
		j++;
		if (j % col == 0)
		{
			cout << '\n';
		}
		
	}

	cout << "NFA图格式:" << endl;
	mark = new bool* [currentId];
	for (i = 0; i < currentId; i++)
	{
		mark[i] = new bool[currentId];
	}
	DFSPrint_table(Nfa.start);

	j = 0;
	for (auto& id : totalCondition)
	{
		if (j >= col)
			cout << id << '\t';
		else
			cout << char(id) << '\t';
		j++;
		if (j % col == 0)
		{
			cout << '\n';
		}
	}
}

void RegularExpression2NFA::DFSPrint_table(NfaNode* root)
{
	if (root == Nfa.end)
		return;

	for (auto& nextNode : root->nextNode)
	{
		if (mark[root->id][nextNode->id] == false)
		{
			mark[root->id][nextNode->id] = true;
			totalCondition[root->id * nextNode->id + nextNode->id] = root->transition[nextNode];
			DFSPrint(nextNode);
		}

	}


}
