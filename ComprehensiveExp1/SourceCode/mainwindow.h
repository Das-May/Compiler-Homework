#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>
#include <string>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    string code;

    /**
     * @brief TypeCount Start at 0
     */
    int TypeCount;
    vector<string> TypeName;
    vector<vector<vector<int>>> MinDFA;

    /**
     * @brief Read regular expression and parse them to MinDFA
     * @param c : The regular expression string of different keywords
     */
    void ProcessRegex(char* c);
    /**
     * @brief Generate c++ code that can be compiled and implemented for lexical analysis
     * @return The code string
     */
    string GenerateCode();

};
#endif // MAINWINDOW_H
