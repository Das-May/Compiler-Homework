#include<iostream>

using namespace std;

enum TokenID {NUMBER,ADD,SUB,MUL,DIV,LBRACKET,RBRACKET,ENDINPUT };


struct TokenStru  // Token �ṹ
{
  TokenID  ID;
  int val;
  char op;
};

struct OperandsNode  // ��Ԫ��������ṹ
{
 bool flag;   // ��������ֵ������ʱ����
 int val;     // ������Ϊֵʱ����ֵ
 char NewVar[10];  // ������Ϊ��ʱ����ʱ������ʱ������
};

OperandsNode exp();
OperandsNode term();
OperandsNode factor();

TokenStru token;

int varOrder=1;  // ��¼��ǰ׼����������ʱ�������


char buffer[255];
int pos=0;

void GetToken()
{
   int s;

   while (buffer[pos]==32||buffer[pos]==9)  // spacebar or tab 
	   pos++; 

   if((buffer[pos]>='0' )&& (buffer[pos]<='9'))  // digit
   {
	   s=0;
	   while (buffer[pos]>='0' && buffer[pos]<='9') 
	   {  s=s*10+buffer[pos]-'0';   pos++ ; } 
       token.ID=NUMBER; token.val=s;  
   }
   else if(buffer[pos]!='\0')
   {
     switch(buffer[pos])
	 {
       case '+': token.ID=ADD; token.op=buffer[pos];  break;  // +
	   case '-': token.ID=SUB; token.op=buffer[pos];  break; // -
	   case '*': token.ID=MUL; token.op=buffer[pos];  break;   // *
	   case '/': token.ID=DIV; token.op=buffer[pos];  break;   // /
	   case '(': token.ID=LBRACKET; token.op=buffer[pos];  break; 
	   case ')': token.ID=RBRACKET; token.op=buffer[pos];  break;
	   default:  cout<<" Error Input at: "<<pos+1; exit(1);   
	 }
	 pos++;
   }
   else  token.ID=ENDINPUT; 

}

void match(TokenID expecttokenid )
{
   if (token.ID== expecttokenid)
        GetToken();
   else{cout<<" Error Input at position: "<<pos+1; exit(1);}

}

void OutputOperand(OperandsNode x)  // ���һ��������
{

    cout<<" , ";
	if (x.flag==0)   // ������ֵ���ֵ������ʱ��������Ϊ������
		cout<<x.val;
	else
	   cout<<x.NewVar;
}

void Gen(TokenStru token,OperandsNode x,OperandsNode y,OperandsNode z)  // ����һ����Ԫ��
{
	cout<<"(";    
	cout<<token.op;   // ��������룬�������
	OutputOperand(x);  // ��һ��������
	OutputOperand(y);   // �ڶ���������
	OutputOperand(z);   // ������������
	cout<<" ) ";
	cout<<endl;

}

OperandsNode exp()
{ 
   TokenStru op;
   char str[5];
   OperandsNode x,y,z;

   x=term();   // ��ȡһ��������

   while(token.ID==ADD || token.ID==SUB)
   { 
	  op=token;
	  match(token.ID);  
      y=term();    // ��ȡһ��������

      z.flag=1;strcpy(z.NewVar,"t");  // ����һ���µ���ʱ����
	  itoa(varOrder,str,10);strcat(z.NewVar,str);

      varOrder++;// ������һ������ʱ���������+1
      Gen(op,x,y,z);    //����һ����Ԫ��
      x=z; 
   }
   return x;
}

OperandsNode term()
{   
   TokenStru op;

   char str[5];
   OperandsNode x,y,z;
   
   x=factor();// ��ȡһ��������

   while(token.ID==MUL || token.ID==DIV)
   { 
      op=token;
	  match(token.ID);  
      y=factor();// ��ȡһ��������

      z.flag=1;strcpy(z.NewVar,"t");   // ����һ���µ���ʱ����
	  itoa(varOrder,str,10);strcat(z.NewVar,str);

      varOrder++;  // ������һ������ʱ���������+1
      Gen(op,x,y,z);  //����һ����Ԫ��
      x=z; 
   }
   return x;
}

OperandsNode factor()
{
   OperandsNode x;

  switch(token.ID)
  {
    case LBRACKET:  match(LBRACKET);
	                x=exp();   // ����һ��������
				    match(RBRACKET);
	                break;
	case NUMBER:    x.flag=0; x.val=token.val;  // ����һ��������
					match(NUMBER); 
					break;
	default: { cout<<"error input at position: " <<pos+1;  exit(1); }

  }
  return x;
}


void main()
{
  cin.getline(buffer,255);
  pos=0;
  
  GetToken();
  
  exp();

  if ( token.ID==ENDINPUT) 
      cout<<"The result is correct! " <<endl;
  else cout<<"error input at position: " <<pos+1;

  system("pause");

}