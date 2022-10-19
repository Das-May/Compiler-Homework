#include "RegularExpression2NFA.h"


void RegularExpression2NFA::GetNFA(string RegularExpression)
{
	currentId = 0;
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
			if (std::count(totalCondition.begin(), totalCondition.end(), c) == 0)// ��������л���¼�����ת������
				totalCondition.push_back(c);//��¼
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
				s.pop(); // ����'('
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
	PrintNfaTable();
}

NfaChunk RegularExpression2NFA::And(char c)
{
	// �����ַ������������NFA���
	NfaChunk andChunk;
	NfaNode* midNode = new NfaNode();

	andChunk.start->id	= currentId;					// ����id
	midNode		  ->id	= currentId + 1;
	andChunk.end  ->id	= currentId + 2;
	currentId += 3;
		
	andChunk.start->nextNode.push_back(midNode);	//����
	andChunk.start->transition[midNode] = '#';
	midNode->nextNode.push_back(andChunk.end);
	midNode->transition[andChunk.end] = c;
		
	return andChunk;
}

NfaChunk RegularExpression2NFA::Or(NfaChunk a, NfaChunk b)
{
	// ��������㣬������a������
	NfaNode* newStartNode = new NfaNode();
	newStartNode->id = currentId;
	currentId++;
	newStartNode->nextNode.push_back(a.start);
	newStartNode->transition[a.start] = '#';
	//��a������δ��֧�㣬����b��ĵڶ����ڵ�
	a.start->nextNode.push_back(b.start->nextNode[0]);
	a.start->transition[b.start->nextNode[0]] = '#';
	delete b.start;
	// �������յ�,����a���b����յ�㼯��ȥ
	NfaNode* newEndNode = new NfaNode();
	newEndNode->id = currentId;
	currentId++;
	a.end->nextNode.push_back(newEndNode);
	a.end->transition[newEndNode] = '#';
	b.end->nextNode.push_back(newEndNode);
	b.end->transition[newEndNode] = '#';
	// �����¿�
	a.start = newStartNode;
	a.end = newEndNode;
	return a;
}

NfaChunk RegularExpression2NFA::Expand_zero(NfaChunk a)
{
	// *
	// ��ͷ��,ת������Ϊepsilon
	NfaNode* midNode = a.start->nextNode[0];	
	a.end->nextNode.push_back(midNode);
	a.end->transition[midNode] = '#';
	// �������,ת������Ϊepsilon
	NfaNode* newEndNode = new NfaNode();		
	newEndNode->id		= currentId;
	currentId++;
	a.end->nextNode.push_back(newEndNode);
	a.end->transition[newEndNode] = '#';
	// �����¿�
	a.end = newEndNode;
	return a;
}

NfaChunk RegularExpression2NFA::Expand_one(NfaChunk a)
{
	// +

	return Connect(a, Expand_zero(a));//���
}

NfaChunk RegularExpression2NFA::Connect(NfaChunk head, NfaChunk tail)
{
	head.end->nextNode   = tail.start->nextNode;// ����
	head.end->transition = tail.start->transition;
	delete tail.start;
	head.end = tail.end;						// ����
	return head;
}

void RegularExpression2NFA::PrintNfaGraph()
{
	cout << "NFAͼ��ʽ:" << endl;
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

void RegularExpression2NFA::PrintNfaTable()
{
	// ��ʼ���������
	int col = totalCondition.size();
	int row = currentId;
	int i, j;
	vector<int> temp;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			Nfa_Table.push_back(temp);
		}
	}


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
	}
	
	cout << "\nNFAͼ��ʽ:" << endl;
	mark = new bool* [currentId];
	for (i = 0; i < currentId; i++)
	{
		mark[i] = new bool[currentId];
		for (j = 0; j < currentId; j++)
			mark[i][j] = false;
	}
	DFSPrint_table(Nfa_Graph.start);

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
	}
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
