#include "GrammarAnalyst.h"
#include "GrammarUtilities.h"
#include <iostream>

void GrammarAnalyst::Init(const vector<vector<int>>& LL1Table, const map<int, string> ID2Word)
{
    this->LL1Table = LL1Table;
    this->ID2Word = ID2Word;

    for(auto c : ID2Word)
    {
        if(c.second == "number")
            NumberID = c.first;
        else if(c.second == "identifier")
            IdentiferID = c.first;
    }

    for(int i = 1; i < LL1Table[0][1]; i++)
        Terminal2Col[ LL1Table[i][0] ] = i;
}

string GrammarAnalyst::Execute(char *code)
{
    if(LL1Table.size() == 0)
        return "Error: have not Init() yet before Execute()\n";

    Buffer = code;
    Pos = 0;
    Output = "";

    GetToken();

    Root = BuildGrammarTree(0);// 起始节点的ID = 0
    if(Token.ID != -1)
        Output = "Finished!";

    return Output;
}

TreeNode* GrammarAnalyst::GetRoot()
{
    return Root;
}

void GrammarAnalyst::GetGrammarTree(TreeNode* node, int deepth)
{
    if(node == 0)
        return;
    for(int i = 0; i < deepth; i++)
        Output += "  ";

    Output += node->Symbol  + '\n' ;

    for(auto child : node->Child)
        GetGrammarTree(child, deepth+1);
}

void GrammarAnalyst::GetToken()
{
    while(GrammarUtil::IsSpace(Buffer[Pos]))
        Pos++;

    string tempWord = "";

    // number
    if(Buffer[Pos] >= '0' && Buffer[Pos] <= '9')
    {
        while(Buffer[Pos] >= '0' && Buffer[Pos] <= '9')
            tempWord += Buffer[Pos++];
        Token.ID = NumberID;
        Token.Word = tempWord;
        return;
    }

    // identifer
    else if(Buffer[Pos] >= 'A' && Buffer[Pos] <= 'z')
    {
        while(Buffer[Pos] >= 'A' && Buffer[Pos] <= 'z')
            tempWord += Buffer[Pos++];
        Token.ID = IdentiferID;
        for(auto terminal : ID2Word)
            if(terminal.first > 100 && terminal.second == tempWord)
                Token.ID = terminal.first;
        Token.Word = tempWord;
        return;
    }

    // other
    for(auto terminal : ID2Word)
        if(terminal.first > 100 && terminal.second[0] == Buffer[Pos])
        {
            for(char c : terminal.second)
            {
                if(Buffer[Pos] == c)
                    tempWord += Buffer[Pos++];
                else
                    Error();
            }
            if(Token.ID != -1)
            {
                Token.ID = terminal.first;
                Token.Word = tempWord;
            }
            return;
        }

    // end
    Token.ID = 101;
    Token.Word = "END";
}

TreeNode* GrammarAnalyst::BuildGrammarTree(int ID)
{
    TreeNode *node = nullptr;

    if(ID >= 100)
    {
        cout << Token.Word << endl;
        cout.flush();

        node = new TreeNode(Token.Word);
        //node = new TreeNode(to_string(Token.ID));
        Match(ID);
    }
    else if(Token.ID != -1)
    {
        int row = ID + 1;
        int col = Terminal2Col[Token.ID];
        int LL1TableCol = LL1Table[0][1];

        vector<int> grammar = LL1Table[row * LL1TableCol + col];
        if(grammar.size() > 0)
        {
            // common grammar tree
//          node = new TreeNode(ID2Word[ID]);
//          for(int i = 1; i < grammar.size(); i++)
//              node->Child.push_back(BuildGrammarTree(grammar[i]));

            // abstract grammar tree
            node = BuildGrammarTree(grammar[1]);
            for(int i = 2; i < grammar.size(); i++)
                node->Child.push_back(BuildGrammarTree(grammar[i]));
        }
    }

    return node;
}

void GrammarAnalyst::Match(int ExpectTokenId)
{
    if (Token.ID == ExpectTokenId)
        GetToken();
    else
        Error();
}

void GrammarAnalyst::Error()
{
    Output = "error input at position: " + to_string(Pos + 1) +
            "\nbuffer[pos] = '" + Buffer[Pos] + "'";
    cout << Output;
    Token.ID = -1;
    Token.Word = "ERROR";
}
