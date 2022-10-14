#include "RegularExpression2NFA.h"


void RegularExpression2NFA::GetNFA(string RegularExpression)
{
	currentId = 0;

	stack<NfaChunk> op;
	string temp1 = "", temp2 = "";

	NfaChunk tempChunk;
	for(auto& c : RegularExpression)
	{
		if (isalpha(c))
		{
			op.push(And(c));
		}
		else 
		{
			switch (c)
			{
			case '*':
				tempChunk = Expand_zero(op.top());
				op.pop();
				op.push(tempChunk);
				break;
			default:
				break;
			}
		}
	}

	Nfa = op.top();
	op.pop();
	while (!op.empty())
	{
		Nfa = Conetct(op.top(), Nfa);
		op.pop();
	}

	PrintNfaGraph();
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

NfaChunk RegularExpression2NFA::Expand_zero(NfaChunk a)
{
	// *

	NfaNode* midNode = a.start->nextNode[0];	// 回头绕,转移条件为epsilon
	a.end->nextNode.push_back(midNode);
	a.end->transition[midNode] = '#';

	NfaNode* newEndNode = new NfaNode();		// 往后大绕,转移条件为epsilon
	newEndNode->id		= currentId;
	currentId			+= 1;
	a.start->nextNode.push_back(newEndNode);
	a.start->transition[newEndNode] = '#';
	a.end = newEndNode;

	return a;
}

NfaChunk RegularExpression2NFA::Conetct(NfaChunk head, NfaChunk tail)
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
	DFSPrint(Nfa.start);
}

void RegularExpression2NFA::DFSPrint(NfaNode* root)
{
	if (root == Nfa.end)
		return;

	for (auto& nextNode : root->nextNode)
	{
		cout << root->id << "---" << root->transition[nextNode] << "--->" << nextNode->id << endl;
		DFSPrint(nextNode);
	}
		

}