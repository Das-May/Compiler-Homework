#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>

#include "GrammarProcessor.h"
#include "GrammarAnalyst.h"

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

    GrammarProcessor *GrammarProcessorInstance;
    GrammarAnalyst GrammarAnalystInstance;

    void SetLL1Table();
    QTreeWidgetItem* GetGrammarTree(TreeNode* root);

};
#endif // MAINWINDOW_H
