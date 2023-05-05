#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

int TypeCount;
vector<string> TypeName;
vector<vector<vector<int>>> MinDFA;
string Output = "";

void Init();
void PrintTable(const vector<vector<int>>& FATable);
bool CheckEnd(int CurrentStauts, const vector<int>& MinDFAInfo);
bool TransferStatus(char*& code, const vector<vector<int>>& MinDFA);

int main()
{
	Init();
	int i = 0;
	for (auto& table : MinDFA)
	{
		cout << "=== " << TypeName[i++] << " === " << endl;
		PrintTable(table);
	}

	cout << "请输入源代码路径:";

	string CodeFilePath;
	cin >> CodeFilePath;
	ifstream  Reader;
	Reader.open(CodeFilePath, ios::in);
	if (!Reader.is_open())
	{
		cout << "无法找到这个文件！请检查路径拼写是否有误" << endl;
		return 0;
	}

	Reader.seekg(0, ios::end);
	int len = Reader.tellg();
	Reader.seekg(0, ios::beg);
	char* code = new char[len + 1];
	memset(code, 0, len + 1);
	Reader.read(code, len);
	cout << "读入的源码为:\n" << code << '\n';
	Reader.close();

	while (*code)
	{
		for (int i = 0; i < TypeCount; i++)
			if (TransferStatus(code, MinDFA[i]))
				Output += "  " + TypeName[i] + '\n';

		code++;
	}
	cout << "单词编码如下:\n" << Output;
	cout << "保存为result.lex\n";

	ofstream writer;
	writer.open("result.lex");
	writer << Output;
	writer.close();

	system("pause");

	return 0;
}

void PrintTable(const vector<vector<int>>& FATable)
{
	if (FATable.size() > 200)
	{
		cout << "DFA内容过多，不便打印\n";
		return;
	}
		
	int row = FATable[0][0];
	int col = FATable[0][1];
	string output = "";
	output += "起始节点ID:" + to_string(FATable[0][2]) + "\n终态节点ID:" + to_string(FATable[0][3]);
	for (int i = 4; i < FATable[0].size(); i++)
		output += ',' + to_string(FATable[0][i]);
	output += '\n';
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			for (int id : FATable[i * col + j])
			{
				if (i == 0 && j == 0)
					continue;
				if (i == 0 && j != 0)
					if (id == 1)
						output += "any";
					else
						output += id;
				else
					output += to_string(id) + ",";
			}
			output += "\t";
		}
		output += '\n';
	}
	cout << output;
	cout.flush();

	int i = 0;
}

bool CheckEnd(int CurrentStauts, const vector<int>& MinDFAInfo)
{
	for (int i = 3; i < MinDFAInfo.size(); i++)
		if (CurrentStauts == MinDFAInfo[i])
			return true;

	return false;
}

bool TransferStatus(char*& code, const vector<vector<int>>& MinDFA)
{
	char* SourcePos = code;
	int MinDFACol = MinDFA[0][1];
	int startID = MinDFA[0][2];
	int CurrentStatusID = startID;
	bool movable = true;

	while (*code)
	{
		// 转移状态
		int col;			// 查询当前字符对应的转移条件
		int AnyCol = -1;	// 若转移条件中存在任意符号指代符'.'，则记录这个转移条件所在的列

		for (col = 1; col < MinDFACol; col++)
		{
			if (MinDFA[col][0] == *code && MinDFA[CurrentStatusID * MinDFACol + col].size() == 1)
				break;
			else if (MinDFA[col][0] == 1)
				AnyCol = col;
		}
		if (col < MinDFACol)
		{
			CurrentStatusID = MinDFA[CurrentStatusID * MinDFACol + col][0];
			code++;
			continue;// 能转移，继续操作
		}
		
		
		if (AnyCol != -1 && MinDFA[CurrentStatusID * MinDFACol + AnyCol].size() == 1)
		{
			CurrentStatusID = MinDFA[CurrentStatusID * MinDFACol + AnyCol][0];
			code++;
			continue;// 能转移，继续操作
		}

		// 状态无法转移
		if (CheckEnd(CurrentStatusID, MinDFA[0]))// 到达终态节点,则输出
		{
			while (SourcePos != code)
			{
				Output += *SourcePos;
				SourcePos++;
			}
			return true;
		}

		code = SourcePos;
		return false;
	}
}

void Init()
{
	TypeCount = 5;
	TypeName.push_back("KEYWORD");
	TypeName.push_back("OPERATOR");
	TypeName.push_back("IDENTIFER");
	TypeName.push_back("NUMBER");
	TypeName.push_back("COMMENT");
	vector<int> cell;
	vector<vector<int>> table;
	table.clear();
	cell.clear(); cell.push_back(8); cell.push_back(8); cell.push_back(2); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(102); table.push_back(cell);
	cell.clear(); cell.push_back(108); table.push_back(cell);
	cell.clear(); cell.push_back(111); table.push_back(cell);
	cell.clear(); cell.push_back(97); table.push_back(cell);
	cell.clear(); cell.push_back(116); table.push_back(cell);
	cell.clear(); cell.push_back(105); table.push_back(cell);
	cell.clear(); cell.push_back(110); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(2); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(5); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(3); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(7); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(6); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(5); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(7); table.push_back(cell);
	cell.clear(); cell.push_back(6); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(3); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(7); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	MinDFA.push_back(table);
	table.clear();
	cell.clear(); cell.push_back(5); cell.push_back(14); cell.push_back(2); cell.push_back(1); cell.push_back(3); cell.push_back(4); table.push_back(cell);
	cell.clear(); cell.push_back(43); table.push_back(cell);
	cell.clear(); cell.push_back(45); table.push_back(cell);
	cell.clear(); cell.push_back(42); table.push_back(cell);
	cell.clear(); cell.push_back(47); table.push_back(cell);
	cell.clear(); cell.push_back(37); table.push_back(cell);
	cell.clear(); cell.push_back(62); table.push_back(cell);
	cell.clear(); cell.push_back(60); table.push_back(cell);
	cell.clear(); cell.push_back(61); table.push_back(cell);
	cell.clear(); cell.push_back(33); table.push_back(cell);
	cell.clear(); cell.push_back(38); table.push_back(cell);
	cell.clear(); cell.push_back(124); table.push_back(cell);
	cell.clear(); cell.push_back(94); table.push_back(cell);
	cell.clear(); cell.push_back(126); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(2); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(3); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(3); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(4); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	MinDFA.push_back(table);
	table.clear();
	cell.clear(); cell.push_back(3); cell.push_back(64); cell.push_back(2); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(95); table.push_back(cell);
	cell.clear(); cell.push_back(97); table.push_back(cell);
	cell.clear(); cell.push_back(98); table.push_back(cell);
	cell.clear(); cell.push_back(99); table.push_back(cell);
	cell.clear(); cell.push_back(100); table.push_back(cell);
	cell.clear(); cell.push_back(101); table.push_back(cell);
	cell.clear(); cell.push_back(102); table.push_back(cell);
	cell.clear(); cell.push_back(103); table.push_back(cell);
	cell.clear(); cell.push_back(104); table.push_back(cell);
	cell.clear(); cell.push_back(105); table.push_back(cell);
	cell.clear(); cell.push_back(106); table.push_back(cell);
	cell.clear(); cell.push_back(107); table.push_back(cell);
	cell.clear(); cell.push_back(108); table.push_back(cell);
	cell.clear(); cell.push_back(109); table.push_back(cell);
	cell.clear(); cell.push_back(110); table.push_back(cell);
	cell.clear(); cell.push_back(111); table.push_back(cell);
	cell.clear(); cell.push_back(112); table.push_back(cell);
	cell.clear(); cell.push_back(113); table.push_back(cell);
	cell.clear(); cell.push_back(114); table.push_back(cell);
	cell.clear(); cell.push_back(115); table.push_back(cell);
	cell.clear(); cell.push_back(116); table.push_back(cell);
	cell.clear(); cell.push_back(117); table.push_back(cell);
	cell.clear(); cell.push_back(118); table.push_back(cell);
	cell.clear(); cell.push_back(119); table.push_back(cell);
	cell.clear(); cell.push_back(120); table.push_back(cell);
	cell.clear(); cell.push_back(121); table.push_back(cell);
	cell.clear(); cell.push_back(122); table.push_back(cell);
	cell.clear(); cell.push_back(65); table.push_back(cell);
	cell.clear(); cell.push_back(66); table.push_back(cell);
	cell.clear(); cell.push_back(67); table.push_back(cell);
	cell.clear(); cell.push_back(68); table.push_back(cell);
	cell.clear(); cell.push_back(69); table.push_back(cell);
	cell.clear(); cell.push_back(70); table.push_back(cell);
	cell.clear(); cell.push_back(71); table.push_back(cell);
	cell.clear(); cell.push_back(72); table.push_back(cell);
	cell.clear(); cell.push_back(73); table.push_back(cell);
	cell.clear(); cell.push_back(74); table.push_back(cell);
	cell.clear(); cell.push_back(75); table.push_back(cell);
	cell.clear(); cell.push_back(76); table.push_back(cell);
	cell.clear(); cell.push_back(77); table.push_back(cell);
	cell.clear(); cell.push_back(78); table.push_back(cell);
	cell.clear(); cell.push_back(79); table.push_back(cell);
	cell.clear(); cell.push_back(80); table.push_back(cell);
	cell.clear(); cell.push_back(81); table.push_back(cell);
	cell.clear(); cell.push_back(82); table.push_back(cell);
	cell.clear(); cell.push_back(83); table.push_back(cell);
	cell.clear(); cell.push_back(84); table.push_back(cell);
	cell.clear(); cell.push_back(85); table.push_back(cell);
	cell.clear(); cell.push_back(86); table.push_back(cell);
	cell.clear(); cell.push_back(87); table.push_back(cell);
	cell.clear(); cell.push_back(88); table.push_back(cell);
	cell.clear(); cell.push_back(89); table.push_back(cell);
	cell.clear(); cell.push_back(90); table.push_back(cell);
	cell.clear(); cell.push_back(48); table.push_back(cell);
	cell.clear(); cell.push_back(49); table.push_back(cell);
	cell.clear(); cell.push_back(50); table.push_back(cell);
	cell.clear(); cell.push_back(51); table.push_back(cell);
	cell.clear(); cell.push_back(52); table.push_back(cell);
	cell.clear(); cell.push_back(53); table.push_back(cell);
	cell.clear(); cell.push_back(54); table.push_back(cell);
	cell.clear(); cell.push_back(55); table.push_back(cell);
	cell.clear(); cell.push_back(56); table.push_back(cell);
	cell.clear(); cell.push_back(57); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(2); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	MinDFA.push_back(table);
	table.clear();
	cell.clear(); cell.push_back(3); cell.push_back(12); cell.push_back(2); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(48); table.push_back(cell);
	cell.clear(); cell.push_back(49); table.push_back(cell);
	cell.clear(); cell.push_back(50); table.push_back(cell);
	cell.clear(); cell.push_back(51); table.push_back(cell);
	cell.clear(); cell.push_back(52); table.push_back(cell);
	cell.clear(); cell.push_back(53); table.push_back(cell);
	cell.clear(); cell.push_back(54); table.push_back(cell);
	cell.clear(); cell.push_back(55); table.push_back(cell);
	cell.clear(); cell.push_back(56); table.push_back(cell);
	cell.clear(); cell.push_back(57); table.push_back(cell);
	cell.clear(); cell.push_back(46); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(2); table.push_back(cell);
	cell.clear(); cell.push_back(2); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	MinDFA.push_back(table);
	table.clear();
	cell.clear(); cell.push_back(4); cell.push_back(3); cell.push_back(2); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(47); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); cell.push_back(2); table.push_back(cell);
	cell.clear(); cell.push_back(3); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	cell.clear(); cell.push_back(3); table.push_back(cell);
	cell.clear(); cell.push_back(1); table.push_back(cell);
	cell.clear(); table.push_back(cell);
	MinDFA.push_back(table);
}
