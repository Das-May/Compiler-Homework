// 3.�������ʽת��Ϊ���ָ�ջʽ������ɫ�Ļ�����ԣ�

#include<iostream>

using namespace std;

// ���������ö��
enum TokenID {NUMBER,ADD,SUB,MUL,DIV,LBRACKET,RBRACKET,ENDINPUT };

// ���ʽṹ
struct TokenStru
{
  TokenID  ID;
  int val;
  char op;
};


void exp();
void term();
void factor();

TokenStru token;

char buffer[255];
int pos=0;


// �ʷ�����������Ŀǰֻ���������ʽ�ʷ��������ܣ�
// �����ʷ��Ŀ��Բο�����ԭ��ڶ�������
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


// ƥ�亯��
void match(TokenID expecttokenid )
{
   if (token.ID== expecttokenid)
        GetToken();
   else{cout<<" Error Input at position: "<<pos+1; exit(1);}

}

void exp()
{ 
   TokenID op;

   term();

   while(token.ID==ADD || token.ID==SUB)
   { 
	  op=token.ID;
	  match(token.ID);  
      term();
	  if(op==ADD)cout<<"ADD"<<endl;
	  else  cout<<"SUB"<<endl;

   }
}

void term()
{   
   TokenID op;
   
   factor();

   while(token.ID==MUL || token.ID==DIV)
   { 
      op=token.ID;
	  match(token.ID);  
      factor();
	  if(op==MUL)cout<<"MUL"<<endl;
	  else  cout<<"DIV"<<endl;
   }
}

void factor()
{

  switch(token.ID)
  {
    case LBRACKET:  match(LBRACKET);
	                exp();
				    match(RBRACKET);
	                break;
	case NUMBER:    cout<<"LDC  ";
		            cout<< token.val  <<endl;
					match(NUMBER); 
					break;
	default: { cout<<"error input at position: " <<pos+1;  exit(1); }

  }
  
}


void main()
{
  cin.getline(buffer,255);// ����һ�е��������ʽ
  pos=0;
  
  GetToken();// ��ȡ��һ����
  
  exp();

  if ( token.ID==ENDINPUT) 
      cout<<"The result is correct! " <<endl;
  else cout<<"error input at position: " <<pos+1;

  system("pause");

}