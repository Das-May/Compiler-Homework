
#include <iostream>
#include <cstring>
#include "RegularExpression2NFA.h"

int main()
{
    
    string s;
    cout << "请输入正则表达式:";
    cin >> s;
    RegularExpression2NFA re;
    cout << re.GetNFA(s);
    cout << re.GetDFA();
    cout << re.GetMinDFA();
    system("pause");
    return 0;
}

/*
测试数据：
a
abc
a*
a?
a|b

a(a|b)
a?(ac|b)*
*/