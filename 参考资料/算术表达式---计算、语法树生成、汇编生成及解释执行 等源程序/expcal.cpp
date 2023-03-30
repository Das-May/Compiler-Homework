// 1.�������ʽ����

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

int exp();
int term();
int factor();

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

// ���+,-���㺯��
int exp()
{ 
   int x;
   TokenID op;

   x=term();

   while(token.ID==ADD || token.ID==SUB)
   {  op=token.ID;
	  match(token.ID);  
      if(op==ADD )   x=x+term();   
	  if(op==SUB )   x=x-term();   
   }
   return x;
}

// ���*,/���㺯��
int term()
{   int x;
    TokenID op;

   x=factor();

   while(token.ID==MUL || token.ID==DIV)
   {  op=token.ID;
	  match(token.ID);  
      if(op==MUL ) x=x*factor();      
	  if(op==DIV ) x=x/factor();    	  
   }
   return x;
}

// ���(),������������
int factor()
{
  int x;

  switch(token.ID)
  {
    case LBRACKET:  match(LBRACKET);
	                x=exp();
				    match(RBRACKET);
	                break;
	case NUMBER:    x=token.val; match(NUMBER); break;
	default: { cout<<"error input at position: " <<pos+1;  exit(1); }

  }
  return x;
}



void main()
{
  cin.getline(buffer,255); // ����һ�е��������ʽ
  pos=0;
  
  GetToken(); // ��ȡ��һ����
  
  int res=exp();

  if ( token.ID==ENDINPUT) 
     cout<<"The result is : "<<res;   
  else cout<<"error input at position: " <<pos+1;

  system("pause");

}