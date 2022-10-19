
#include <iostream>
#include <cstring>
#include "RegularExpression2NFA.h"

int main()
{
    RegularExpression2NFA re;
    //re.GetNFA("a*");
    re.GetNFA("a*(a|b)");
    return 0;
}

/*
测试数据：
a
abc
a*
a+
a|b

a(a|b)
a*(a|b)+
a(a*|(a|c))
*/