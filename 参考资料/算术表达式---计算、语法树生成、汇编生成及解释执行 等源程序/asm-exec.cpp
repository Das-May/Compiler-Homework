// 4.�������ʽ��Ӧ���ָ��Ľ���ִ��

#include<iostream>
#include<stack>

using namespace std;

// ָ��������������ö��
enum TokenID {LDC,ADD,SUB,MUL,DIV };

// ָ�������������ľ��嵥��
char *Instruction[5]={"LDC","ADD","SUB","MUL","DIV"};

stack<int> st;


// ���ָ��Ľ���ִ�й���
void AsmExec(void)
{


	char buffer[20],Opstr[5],Operands[10];
	int op1,op2;
	int pos;

   while(cin.getline(buffer,20))  // ���ж���ָ�һ��ָ��ռ��һ��
   {
	  strncpy(Opstr,buffer,3);  // ����������ܵ���
	  Opstr[3]='\0';
	  for(int i =0;i<5;i++) 
		  if(strcmp(Instruction[i],Opstr)==0) break;  //˳����ң���ʶ���ܵ������
      switch(i)
	  {
	    case LDC: pos=4;  // ����ָ��
			      while(buffer[pos]!='\0')   // �����������
				  {  Operands[pos-4]=buffer[pos]; pos++;   }
				  Operands[pos-4]='\0';
				  op1=atoi(Operands);  // ת��������Ϊ����
				  st.push(op1);
				  break;
		case ADD: op1=st.top();st.pop();   // �ӷ�ָ��
			      op2=st.top();st.pop();
				  st.push(op1+op2);
				  break;
		case SUB:op1=st.top();st.pop();   // ����ָ��
			      op2=st.top();st.pop();
				  st.push(op2-op1);
				  break;
		case MUL:op1=st.top();st.pop();   // �˷�ָ��
			      op2=st.top();st.pop();
				  st.push(op1*op2);
				  break;
		case DIV:op1=st.top();st.pop();  // ����ָ��
			      op2=st.top();st.pop();
				  st.push(op2/op1);
				  break;
	  }
   }
 
}





void main()
{

	AsmExec();

	cout<<st.top(); 

	system("pause");

}


