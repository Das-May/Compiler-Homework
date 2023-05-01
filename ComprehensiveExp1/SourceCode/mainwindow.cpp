#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Loader.h"
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

#include <Regex2NFA.h>
#include <Automata.h>

#include <string>

#define TW_NFA(index) ui->TW_NFA_##index
#define TW_DFA(index) ui->TW_DFA_##index
#define TW_MinDFA(index) ui->TW_MinDFA_##index

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("编码解析器");

    QString FileName = "D:\\Homework\\Compiler-Homework\\toGithub\\ComprehensiveExp1\\TestData\\TINY.txt";
    QByteArray fn_qba = FileName.toUtf8();
    char* fn_cc = fn_qba.data();
    char* c = Loader::ReadText(fn_cc);

    ProcessRegex(c);

//    connect(ui->Btn_OpenText,&QPushButton::clicked,[=](){
//        QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取正则表达式.txt"),"D:",QStringLiteral("(*txt)"));
//        if(FileName != "")
//        {
//            QByteArray fn_qba = FileName.toUtf8();
//            char* fn_cc = fn_qba.data();
//            char* c = Loader::ReadText(fn_cc);

//            ProcessRegex(c);
//        }
//    }
//    );

    FileName = "D:\\Homework\\Compiler-Homework\\toGithub\\ComprehensiveExp1\\TestData\\TINY.cpp";
    fn_qba = FileName.toUtf8();
    fn_cc = fn_qba.data();
    string code = GenerateCode();
    Loader::WriteText(fn_cc, code.c_str());

    connect(ui->Btn_SaveCode,&QPushButton::clicked,[=](){
        QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("保存路径"),"D:",QStringLiteral("(*txt)"));
        if(FileName != "")
        {
            QByteArray fn_qba = FileName.toUtf8();
            char* fn_cc = fn_qba.data();
            string code = GenerateCode();
            Loader::WriteText(fn_cc, code.c_str());
        }
    }
    );
}

bool IsSeparator(char c)
{
    return (c==' ' || c=='\n' || c=='\r' || c=='\t');
}

void PassSeparator(char *&c)
{
    while(IsSeparator(*c))
        c++;
}

string GetFeild(char *&c)
{
    string temp = "";
    while (!IsSeparator(*c))
    {
        temp += *c;
        c++;
    }
    return temp;
}

void DisplayFATable(QTableWidget*& TW, const vector<vector<int>>& FA)
{
    if(FA.size() > 200)
        return;

    int Row = FA[0][0];
    int Col = FA[0][1];

    TW->setRowCount(Row);
    TW->setColumnCount(Col);

    string info = "起始节点:" + to_string(FA[0][2]) + "\n终态节点:" + to_string(FA[0][3]);
    for(int i = 4; i < FA[0].size(); i++)
        info += ',' + to_string(FA[0][i]);
    TW->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(info)));

    for(int i = 1; i < Col; i++)
        TW->setItem(0, i, new QTableWidgetItem(QString((char)FA[i][0])));

    for(int i = Col; i < Row*Col; i++)
    {
        string item = "";
        bool IsFirst = false;
        for(int j : FA[i])
        {
            if(!IsFirst)
            {
                IsFirst = true;
                item += to_string(j);
            }
            else
                item += ',' + to_string(j);
        }
        TW->setItem(i/Col, i%Col, new QTableWidgetItem(QString::fromStdString(item)));
    }
}

void MainWindow::ProcessRegex(char* c)
{
    TypeCount = 0;
    string regex;
    Regex2NFA regex2NFA;
    vector<vector<int>> NFATable;
    vector<vector<int>> DFATable;
    vector<vector<int>> MinDFATable;
    while(*c)
    {
        PassSeparator(c);

        TypeName.push_back(GetFeild(c));
        ui->tabWidget->setTabText(TypeCount, QString::fromStdString(TypeName[TypeCount]));

        PassSeparator(c);

        regex = GetFeild(c);
        qDebug() << QString::fromStdString(regex) << "is being processing." << endl;

        while(regex.find("letter")!= string::npos)
            regex.replace(regex.find("letter"), 6, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)");
        while(regex.find("digit") != string::npos)
            regex.replace(regex.find("digit"), 5, "(0|1|2|3|4|5|6|7|8|9)");

        NFATable = regex2NFA.GetNFA(regex);

        DFATable = Automata::GetDFA(NFATable);

        MinDFATable = Automata::GetMinDFA(DFATable);
        MinDFA.push_back(MinDFATable);

        // 以下是屎山，不要看
        switch (TypeCount)
        {
            case 0:
                DisplayFATable(ui->TW_NFA_0, NFATable);
                DisplayFATable(ui->TW_DFA_0, DFATable);
                DisplayFATable(ui->TW_MinDFA_0, MinDFATable);
                break;
            case 1:
                DisplayFATable(ui->TW_NFA_1, NFATable);
                DisplayFATable(ui->TW_DFA_1, DFATable);
                DisplayFATable(ui->TW_MinDFA_1, MinDFATable);
                break;
            case 2:
                DisplayFATable(ui->TW_NFA_2, NFATable);
                DisplayFATable(ui->TW_DFA_2, DFATable);
                DisplayFATable(ui->TW_MinDFA_2, MinDFATable);
                break;
            case 3:
                DisplayFATable(ui->TW_NFA_3, NFATable);
                DisplayFATable(ui->TW_DFA_3, DFATable);
                DisplayFATable(ui->TW_MinDFA_3, MinDFATable);
                break;
            case 4:
                DisplayFATable(ui->TW_NFA_4, NFATable);
                DisplayFATable(ui->TW_DFA_4, DFATable);
                DisplayFATable(ui->TW_MinDFA_4, MinDFATable);
                break;
            case 5:
                DisplayFATable(ui->TW_NFA_5, NFATable);
                DisplayFATable(ui->TW_DFA_5, DFATable);
                DisplayFATable(ui->TW_MinDFA_5, MinDFATable);
                break;
            default:
                qDebug() << "too much types of keyword~" << endl;
        }

        qDebug() << QString::fromStdString(regex) << "finished." << endl;
        TypeCount++;

        PassSeparator(c);
    }
}

string MainWindow::GenerateCode()
{
    string code = "";

//    code += "#include <iostream>\
//        #include <sstream>\
//        #include <fstream>\
//        #include <vector>\
//        #include <map>\
//        using namespace std;\
//        \
//        int MinDFARow, MinDFACol;\
//        int TypeCount;\
//        vector<string> TypeName;\
//        vector<vector<vector<int>>> MinDFAs;\
//        vector<int> TypeStatus;\
//        \
//        void Init()\
//        {\n";

    code += "TypeCount = " + to_string(TypeCount) + ";\n";
    for(string name : TypeName)
        code += "TypeName.push_back(\"" + name + "\");\n";
    code += "vector<int> cell;\n\
            vector<vector<int>> table;\n";
    for(const vector<vector<int>>& table : MinDFA)
    {
        code += "table.clear();\n";
        for(const vector<int>& cell : table)
        {
            code += "cell.clear();\n";
            for(const int id : cell)
                code += "cell.push_back(" + to_string(id) + ");\n";
            code += "table.push_back(cell);\n";
        }
        code += "MinDFA.push_back(table);\n";
    }
    return code;

    code += "}\
            \n\
            bool CheckEnd(int CurrentStauts, const vector<int>& MinDFAInfo)\
            {\
                for (int i = 2; i < MinDFAInfo.size(); i++)\
                    if (CurrentStauts == MinDFAInfo[i])\
                        return true;\
                return false;\
            }\
            \n\
            void TransferStatus(int& CurrentStatus, char Condition, const vector<vector<int>>& MinDFA)\
            {\
                MinDFACol = MinDFA[0][1];\
\
                int col;\
                for (col = 1; col < MinDFACol; col++)\
                    if (MinDFA[col][0] == Condition)\
                        break;\
\
                CurrentStatus = MinDFA[CurrentStatus * MinDFACol + col][0];\
            }\
\
            int main()\
            {\
                Init();\
\
                cout << \"请输入源代码路径:\";\
\
                string CodeFilePath;\
                cin >> CodeFilePath;\
                ifstream  Reader;\
                Reader.open(CodeFilePath, ios::in);\
                if (!Reader.is_open())\
                {\
                    cout << \"无法找到这个文件！请检查路径拼写是否有误\" << endl;\
                    return 0;\
                }\
\
                Reader.seekg(0, ios::end);\
                int len = Reader.tellg();\
                Reader.seekg(0, ios::beg);\
                char* code = new char[len + 1];\
                memset(code, 0, len + 1);\
                Reader.read(code, len);\
                //cout << code;\
                Reader.close();\
\
                string Output = "";\
                char buffer;\
                string Word = "";\
                int CurrentType = 0;\
                bool IsEnd = false;\
                while (*code)\
                {\
                    buffer = *code;\
                    Word += buffer;\
\
                    for (int i = 0; i < TypeCount; i++)\
                    {\
                        TransferStatus(TypeStatus[i], buffer, MinDFA[i]);\
                        IsEnd = CheckEnd(TypeStatus[i], MinDFA[i][0]);\
                        if (IsEnd)\
                        {\
                            if (CurrentType != i)\
                            {\
                                Output += Word + " " + TypeName[CurrentType] + '\n';\
\
                                CurrentType = i;\
                            }\
                        }\
                    }\
\
                    code++;\
                }\
\
                ofstream writer;\
                writer.open(\"result.lex\");\
                writer << Output;\
                writer.close();\
\
                return 0;\
            }\n";

    return code;
}

MainWindow::~MainWindow()
{
    delete ui;
}

