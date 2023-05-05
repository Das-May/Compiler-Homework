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

bool IsRegexOperator(char c)
{
    return (c=='|' || c=='*' || c=='?' || c=='(' || c==')');
}

bool IsRegexKeyword(char c)
{
    return (c=='.');
}

void Regex2NFA::GenerateNFA(string Regex)
{
    while(Regex.find("letter")!= string::npos)
        Regex.replace(Regex.find("letter"), 6, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)");
    while(Regex.find("digit") != string::npos)
        Regex.replace(Regex.find("digit"), 5, "(0|1|2|3|4|5|6|7|8|9)");
    Regex = '(' + Regex + ')';

    maxId = 1;
	totalCondition.clear();
	totalCondition.push_back('#');

	stack<NfaChunk> s;
	string temp1 = "", temp2 = "";

	NfaChunk tempChunk, opChunk, tempChunk_2;
    for(int i = 0; i < Regex.size(); i++)
	{
        char c = Regex[i];
        if(c == '\\')// 转义符
        {
            i++;
            s.push(GenerateNFAChunk(Regex[i]));
            if (count(totalCondition.begin(), totalCondition.end(), Regex[i]) == 0)// 如果容器中还记录过这个转移条件
                totalCondition.push_back(Regex[i]);//记录
        }
        else if (IsRegexOperator(c))// 运算符
		{
            switch (c)
            {
                case '*':
                    tempChunk = Expand_zero(s.top());
                    s.pop();
                    break;
                case '?':
                    tempChunk = Choose(s.top());
                    s.pop();
                    break;
                case '(':
                    tempChunk.op = c;
                    break;
                case ')':
                    tempChunk = s.top();
                    s.pop();

                    while (s.top().op != '(')
                    {
                        switch (s.top().op)
                        {
                            case '#':
                                tempChunk = Connect(s.top(), tempChunk);
                                s.pop();
                                break;
                            case '|':
                                opChunk = s.top();
                                s.pop();

                                tempChunk_2 = s.top();
                                s.pop();

                                tempChunk = Or(tempChunk, tempChunk_2);
                                break;
                            default:
                                cout << "Regex Error: invalid ()! current char is '" << c << "', from " << Regex << endl;
                        }
                    }
                    s.pop(); // 弹出'('
                    break;
                case '|':
                    tempChunk.op = c;
                    break;
                default:
                    // never enter
                    break;
            }
            s.push(tempChunk);
		}
        else if (IsRegexKeyword(c))// 特殊字符
        {
            // . -> (char)1
            // 将来如果要拓展特殊字符的种类，这部分应该改为switch-case语句
            s.push(GenerateNFAChunk((char)1));
            if (count(totalCondition.begin(), totalCondition.end(), (char)1) == 0)// 如果容器中还记录过这个转移条件
                totalCondition.push_back((char)1);//记录
        }
        else // 一般字符
		{
            s.push(GenerateNFAChunk(c));
            if (count(totalCondition.begin(), totalCondition.end(), c) == 0)
                totalCondition.push_back(c);
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

NfaChunk Regex2NFA::GenerateNFAChunk(char c)
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
    //以a块的起点末分支点，连接b块的第二个节点
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

NfaChunk Regex2NFA::Choose(NfaChunk a)
{
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
    NFARow = maxId;
    NFACol = totalCondition.size() + 1;
	vector<int> temp;

    for (int i = 0; i < NFARow * NFACol; i++)           // 填充
        NFATable.push_back(temp);
    NFATable[0].push_back(NFARow);             // 第一格 记录 行、列、起点节点id、终态节点的id
    NFATable[0].push_back(NFACol);
    NFATable[0].push_back(NFAGraph.start->id);
    NFATable[0].push_back(maxId-1);
    for(int i = 1; i < NFACol; i++)            // 第一行 记录 转移条件
        NFATable[i].push_back(totalCondition[i-1]);
    for(int i = NFACol; i < NFARow*NFACol; i += NFACol) // 第一列 记录 NFD节点的id
        NFATable[i].push_back(i/NFACol);

    // 初始化遍历标记
    mark = new bool* [maxId];
    for (int i = 0; i < maxId; i++)
    {
        mark[i] = new bool[maxId];
        for (int j = 0; j < maxId; j++)
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
            for (int i = 0; i < totalCondition.size(); i++)
			{
                if (totalCondition[i] == root->transition[nextNode])
                {
                    int row = root->id;
                    int col = i + 1;
                    NFATable[row * (totalCondition.size()+1) + col].push_back(nextNode->id);
                }
			}
            DFSNFA(nextNode);
		}
	}
}
