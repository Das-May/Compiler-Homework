#include "regularExpression_processor.h"

string RegularExpressionProcessor::PrintFATable(vector<vector<char>>::iterator it)
{
    string s;
    if((*it).size() == 0 || (*it).size() > 2)
        s = "不存在";

    else
    {
        // 打印表格
        int row = (*it)[0];
        int col = (*it)[1];
        it++;
        s = to_string(row) + '_' + to_string(col) + '\t';
        for (int i = 1; i < row * col ; i++, it++)
        {
            if (i % col == 0)
                s += '\n';

            for (auto& id : *it)
            {
                s = s + id + ' ';
            }
            s += '\t';
        }
        s += '\n';
    }

    return s;
}

void RegularExpressionProcessor::SetNFA(vector<vector<char>> nfa)
{
    NFA = nfa;
}

string RegularExpressionProcessor::GetNFA()
{
    if(NFA.size() == 0)
        return "不存在";

    return PrintFATable(NFA.begin());
}

string RegularExpressionProcessor::GetDFA()
{
    if(NFA.size() == 0)
        return "不存在";

    // 建立映射表
    map<char, int> m;
    for(int i = NFA[0][1]; i < NFA[0][0] * NFA[0][1]; i += NFA[0][1])
    {
        m[NFA[i][0]] = i;
    }

    // 建立
    queue<vector<char>> colQueue;
    vector<char> latest;
    queue<char> q;
    char currentId;

    // 初始化DFA表头
    int col = NFA[0][1] - 1;
    int row = 1;
    vector<char> temp;
    DFA.push_back(temp);
    for(int i = 1; i < col; i++)
    {
        temp.clear();
        temp.push_back(NFA[i+1][0]);
        DFA.push_back(temp);
    }


    // 从起始符号开始，把S--epsilon-->ID全都放进第二行第一列
    temp.clear();
    temp.push_back(NFA[col +1][0]);
    for(auto &i : NFA[col + 2])
        temp.push_back(i);
    colQueue.push(temp);

    /*循环*/
    while (!colQueue.empty())
    {
        // 取出第一列的预备单元格
        latest = colQueue.front();
        colQueue.pop();

        // 检查第一列中是否已存在相同单元格
        bool repeat = false;
        for (int j = 1; j < row; j++)
        {
            if (latest == DFA[j * col])
            {
                repeat = true;
                break;
            }
        }


        if (repeat == false) //若不存在,则填入第row行的第一列，继续后续的初始化操作
            DFA.push_back(latest);
        else//若存在，则跳过这个单元格
            continue;


        // 初始化后续的条件转移后的id容器
        temp.clear();
        for (int i = 1; i < col; i++)
        {
            DFA.push_back(temp);
        }

        // 遍历DFA第一列的最新一行的所有起点id，获取对应的NFA转移条件的终点id，填写到DFA中【注意去重】
        for (auto& id : latest)
        {
            for (int i = 1; i < col; i++)
            {
                for (auto id2 : NFA[m[id] + i + 1])
                {
                    DFA[row * col + i].push_back(id2);
                }
            }

        }
        for (int i = 1; i < col; i++)//用集合去重
        {
            set<int> s(DFA[row * col + i].begin(), DFA[row * col + i].end());
            DFA[row * col + i].assign(s.begin(), s.end());
        }

        // 遍历条件列的id，追加epsilon能到达的所有id【注意去重】
        for (int i = 1; i < col; i++)
        {
            while (!q.empty())// 清空队列
                q.pop();

            for (auto& id : DFA[row * col + i])//遍历条件列的id
                q.push(id);

            while (!q.empty())//追加epsilon能到达的所有id
            {
                currentId = q.front();
                q.pop();

                for (auto id : NFA[ m[currentId] + 1 ])//epsilon列
                {
                    if (find(DFA[row * col + i].begin(), DFA[row * col + i].end(), id) == false)// 去重：容器中还没有这个id
                    {
                        DFA[row * col + i].push_back(id);//则追加id
                        q.push(id);
                    }

                }
            }
        }

        // 取条件列的容器，放入预备队列中
        for (int i = 1; i < col; i++)
        {
            if(DFA[row * col + i].size() > 0)
                colQueue.push(DFA[row * col + i]);
        }

        row++;

    }
    DFA[0].push_back(row);
    DFA[0].push_back(col);
    return PrintFATable(DFA.begin());
}

string RegularExpressionProcessor::GetMinDFA()
{
    if(DFA.size() == 0)
        return "不存在";

    // 给每行划分子集
    int DFA_row = DFA[0][0];
    int DFA_col = DFA[0][1];
    string s[DFA_row];

    for(int i = 1; i < DFA_row; i++)//第i行
    {
        for(int j = 1; j < DFA_col; j++)//第j列
        {
            if(DFA[DFA_col * i + j].size() == 0)
                s[i] += ' ';
            else
                for(auto & id : DFA[DFA_col * i + j])
                {
                    s[i] += id;
                }
        }
    }
    // 并查集
    int unionset[DFA_row];
    memset(unionset, -1, sizeof (unionset));
    for(int i = 1; i < DFA_row; i++)
    {
        for(int j = i + 1; j < DFA_row; j++)
        {
            if(s[i] == s[j])
            {
                unionset[j] = i;
                unionset[i]--;
            }
        }
    }

    // 初始化minDFA表头(复制粘贴DFA的)
    vector<char> temp;
    int row = 1, col = DFA[0][1];
    for(int i = 0; i < col; i++)
        minDFA.push_back(DFA[i]);

    // 填入内容
    for(int i = 1; i < DFA_row; i++)
    {
        if(unionset[i] < 0)
        {
            for(int j = 0; j < col; j++)
            {
                minDFA.push_back(DFA[col * i + j]);//用DFA的第i行 初始化minDFA的第row行
            }

            if(unionset[i] < -1)//如果后续有需要合并的行
            {
                for(int k = 1; k < DFA_row; k++)//对应DFA中的第k行
                {
                    if(unionset[k] == i)
                    {
                        minDFA[row * col].push_back(DFA[k * col][0]);// 将第一列合并到minDFA的第row行中
                    }
                }
            }

            row++;
        }

    }

    minDFA[0][0] = row;
    minDFA[0][1] = col;
    return PrintFATable(minDFA.begin());
}

bool RegularExpressionProcessor::find(vector<char>::iterator begin, vector<char>::iterator end, char value)
{
    for( vector<char>::iterator it = begin; it != end; it++)
    {
        if((*it) == value)
            return true;
    }
    return false;
}
