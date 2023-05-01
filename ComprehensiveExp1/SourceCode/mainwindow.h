#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <map>
#include <vector>
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

    int TypeCount;
    map<int, string> TypeName;
    map<int, vector<vector<int>>> MinDFA;
    int* TypeStatus;

    void ProcessRegex(char* c);

};
#endif // MAINWINDOW_H
