#include "Loader.h"

char* Loader::Input(const char* filePath, int type)    // type=1,����cpp�ļ���type=0,����bin�ļ�
{
    // ���ļ�
    ifstream file;
    if (type)
        file.open(filePath, ios::in | ios::binary);
    else
        file.open(filePath, ios::in | ios::binary);
    if (!file)
    {
        cerr << "�ļ���ȡʧ�ܣ�" << endl;
        abort();
    }

    // ��¼�ļ�·��
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);
    cout << "�ļ�·���� " << filePath << endl;

    // ��ȡ�ı�����
    int len;
    file.seekg(0, ios::end);
    len = file.tellg();
    cout << "�ļ����ݳ��ȣ� " << len << endl;

    // ��ȡ�ı�����
    file.seekg(0, ios::beg);
    char* c = new char[ len + 1 ];
    memset(c, 0, len + 1);
    file.read(c,len);
    cout << "�ļ����ݣ� " << c << endl;

    //����charָ��
    return c;
}

void Loader::Output(const char* processed, int type)    // type=1,����bin�ļ���type=0������cpp�ļ�
{
    // �����ļ�����·��
    int len = strlen(filePath) - 3;          
    filePath[len] = '\0';                       // �ص�
    if(type)                                    // ׷��
        strcat(filePath, "bin");             
    else
        strcat(filePath, "cpp");
    cout << "�ļ�����·���� " << filePath << endl;

    // �����ļ�
    ofstream file;
    if (type)
        file.open(filePath, ios::out | ios::binary);
    else
        file.open(filePath, ios::out);
    if (!file)    // �����ʧ��
    {
        cerr << "�ļ�д��ʧ�ܣ�" << endl;
        abort();
    }

    file.write(processed, sizeof(processed));
    if (type)
        cout << "ѹ���ɹ���" << endl;
    else
        cout << "��ѹ�ɹ���" << endl;
}
