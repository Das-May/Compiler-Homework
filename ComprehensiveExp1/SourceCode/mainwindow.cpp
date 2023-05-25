#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Loader.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

#include <QDebug>

#include <Regex2NFA.h>
#include <Automata.h>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Btn_OpenText,&QPushButton::clicked,[=](){
        QString FileName = QFileDialog::getOpenFileName(this,tr("选取正则表达式"),"D:",QStringLiteral("text(*.txt)"));
        if(FileName != "")
        {
            QByteArray fn_qba = FileName.toUtf8();
            char* fn_cc = fn_qba.data();
            char* c = Loader::ReadText(fn_cc);

            ProcessRegex(c);

            code = GenerateCode();
            ui->TB_Code->setText(QString::fromStdString(code));
        }
    }
    );

    connect(ui->Btn_SaveCode,&QPushButton::clicked,[=](){
        QString FileName = QFileDialog::getSaveFileName(this,tr("保存路径"),"D:",tr("cpp code(*.cpp)"));
        if(FileName != "")
        {
            QByteArray fn_qba = FileName.toUtf8();
            char* fn_cc = fn_qba.data();
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
    while (!IsSeparator(*c) && *c!=0)
    {
        temp += *c;
        c++;
    }
    return temp;
}

void DisplayFATable(QTableWidget*& TW, const vector<vector<int>>& FA)
{
    if(FA.size() > 200)
    {
        TW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        TW->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        TW->setRowCount(5);
        TW->setColumnCount(1);
        TW->setItem(0, 0, new QTableWidgetItem(QString("DFA内容过多，不便展示")));
        TW->setItem(0, 1, new QTableWidgetItem(QString::fromStdString("行数: " + to_string(FA[0][0]))));
        TW->setItem(0, 2, new QTableWidgetItem(QString::fromStdString("列数: " + to_string(FA[0][1]))));
        TW->setItem(0, 3, new QTableWidgetItem(QString::fromStdString("起始节点: " + to_string(FA[0][2]))));
        string s = "终止节点: " + to_string(FA[0][3]);
        for(int i = 4; i < FA[0].size(); i++)
            s += ',' + to_string(FA[0][i]);
        TW->setItem(0, 4, new QTableWidgetItem(QString::fromStdString(s)));
        return;
    }

    int Row = FA[0][0];
    int Col = FA[0][1];

    TW->setRowCount(Row);
    TW->setColumnCount(Col);

    string info = "起始节点:" + to_string(FA[0][2]) + "\n终态节点:" + to_string(FA[0][3]);
    for(int i = 4; i < FA[0].size(); i++)
        info += ',' + to_string(FA[0][i]);
    TW->setItem(0, 0, new QTableWidgetItem(QString::fromStdString(info)));

    for(int i = 1; i < Col; i++)
        if(FA[i][0] == 1)
            TW->setItem(0, i, new QTableWidgetItem(QString("any")));
        else
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
    while(*c != 0)
    {
        PassSeparator(c);

        TypeName.push_back(GetFeild(c));
        ui->tabWidget->setTabText(TypeCount, QString::fromStdString(TypeName[TypeCount]));

        PassSeparator(c);

        regex = GetFeild(c);
        qDebug() << QString::fromStdString(regex) << "is being processing." << endl;

        NFATable = regex2NFA.GetNFA(regex);

        DFATable = Automata::GetDFA(NFATable);

        MinDFATable = Automata::GetMinDFA(DFATable);
        MinDFA.push_back(MinDFATable);

        // 以下是屎山，请忽略
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
    string code = Loader::ReadText("template.txt");

    code += "TypeCount = " + to_string(TypeCount) + ";\n";

    for(string name : TypeName)
        code += "TypeName.push_back(\"" + name + "\");\n";

    code += "vector<vector<int>> table;\n";
    for(const vector<vector<int>>& table : MinDFA)
    {
        code += "table.clear();\n";
        for(const vector<int>& cell : table)
        {
            string temp;
            for(const int id : cell)
                temp += to_string(id) + ", ";
            code += "table.push_back({" + temp + "});\n";
        }
        code += "MinDFA.push_back(table);\n";
    }

    code += "}";

    return code;
}

MainWindow::~MainWindow()
{
    delete ui;
}

