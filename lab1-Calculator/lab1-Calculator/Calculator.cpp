/*
Simple calculator

Revision history:
	Revised by Kang Huquan October 2018
	Originally written by Bjarne Stroustrup
		(bs@cs.tamu.edu)Spring 2004.

This program implements a basic expression calculator.
Input from input.txt;output to output.txt.

The grammar for input is:

Statement:
	Expression
	Print

Print:
	;
Expression:
	Term
	Expression + Term
	Expression - Term
Term:
	Primary
	Term * Primary
	Term / Primary
	Term % Primary
Primary:
	Number
	(Expression)
	- Primary
	+ Primary
	Primary !
Number:
	floating-point-literal

Input comes from infile through the Token_stream called ts.
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const char number = '8';
const char print = ';';

//���������������ĵ��ʣ������ͺ�ֵ
class Token
{
public:
	char kind;
	double value;
	Token(char ch)
		:kind(ch), value(0) {}
	Token(char ch,double val)
		:kind(ch),value(val){}
};

//���ʻ����������ڴ�������
class Token_stream
{
private:
	bool full;
	Token buffer;
public:
	Token_stream()
		:full(false), buffer(0){}
	void putback(Token t);
	void ignore(char c);
	Token get();
};

Token_stream ts;//��������ȫ�ֱ���
double ANS = 0;//�����ϴμ�����
ifstream infile("input.txt");
ofstream outfile("output.txt");

//�����׳��쳣
void error(string s)
{
	outfile << "error" << endl;
	throw runtime_error(s);
}

//����׳�
double factorial(double d)
{
	int i = int(d);
	if (i != d)error("factorial: got a float-point number");
	if (i < 0)error("factorial:got a minus number");
	int result = 1;
	for (int j = 1; j <= i; j++)
		result *= j;
	return double(result);
}

double expression();//��������

//�ķ���
//Primary :
//	Number
//	(Expression)
//	- Primary
//	+ Primary
//	Primary !
//Number :
//	floating - point - literal
double primary()
{
	double result;
	Token t = ts.get();
	switch (t.kind)
	{
	case'(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind == print)infile.putback(print);
		if (t.kind != ')')error("')'expected");
		result= d;
		break;
	}
	case number:
		result= t.value;
		break;
	case'-':
		result= -primary();
		break;
	case'+':
		result= primary();
		break;
	default:
		error("primary expected");
	}
	t = ts.get();
	while (t.kind == '!') {
		result= factorial(result);
		t = ts.get();
	}
	ts.putback(t);
	return result;
}
//�ķ���
//Term :
//	Primary
//	Term * Primary
//	Term / Primary
//	Term % Primary
double term()
{
	double left = primary();
	Token t = ts.get();

	while (true) {
		switch (t.kind)
		{
		case'*':
			left *= primary();
			t = ts.get();
			break;
		case'/':
		{
			double d = primary();
			if (d == 0)error("divide by zero");
			left /= d;
			t = ts.get();
			break;
		}
		case'%':
		{
			double d = primary();
			int i1 = int(left);
			if (i1 != left)error("left-hand operand of % not int");
			int i2 = int(d);
			if (i2 != d)error("right-hand operand of % not int");
			if (i2 == 0)error("%:divide by zero");
			left = i1 % i2;
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}
//�ķ���
//Expression:
//	Term
//	Expression + Term
//	Expression - Term
double expression()
{
	double left = term();
	Token t = ts.get();

	while (true) {
		switch (t.kind)
		{
		case'+':
			left += term();
			t = ts.get();
			break;
		case'-':
			left -= term();
			t = ts.get();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

//����������н�������;��֮ǰ�Ļ����ַ�
void clean_up_mess()
{
	ts.ignore(print);
}

//������ѭ��
void calculate()
{
	while (infile) {
		char ch;
		infile >> ch;
		if (infile.fail()) break;//�ж��ļ���ȡ�Ƿ����
		infile.putback(ch);//�˻������ж��ַ�

		try {
			Token t = ts.get();//��ȡ�����ַ�
			while (t.kind == print)t = ts.get();//������ӡ�ַ���;��
			ts.putback(t);//�˻ز����ַ�
			ANS = expression();//������ʽ���
			t = ts.get();//��ȡ�����ַ�
			if (t.kind != print)error("mess on the bottom");//�жϱ��ʽ���Ƿ��зǷ��ַ�
			ts.putback(t);//�˻ز����ַ�
			outfile << ANS << endl;
		}
		catch (runtime_error) {
			clean_up_mess();
		}
	}
}

int main()
{
	calculate();
	return 0;
}

//�˻�һ������
void Token_stream::putback(Token t)
{
	if (full)
		error("putback()into a full buffer");
	buffer = t;
	full = true;
}

//���������ַ���ֱ�������ַ�c��cҲ������
void Token_stream::ignore(char c)
{
	if (full&&c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0;
	while (infile >> ch)
		if (ch == c)return;
}

//��ȡһ�����ʣ���Ч�ĵ��ʰ�����()+-*/%!ANS������
Token Token_stream::get()
{
	if (full) {
		full = false;
		return buffer;
	}
	char ch;
	infile >> ch;

	switch (ch)
	{
	case print:
	case'(':
	case')':
	case'+':
	case'-':
	case'*':
	case'/':
	case'%':
	case'!':
		return Token(ch);
	case'.':
	case'0':case'1':case'2':case'3':case'4':
	case'5':case'6':case'7':case'8':case'9':
	{
		infile.putback(ch);
		double val;
		infile >> val;
		return Token(number, val);
	}
	case'A':
	{
		char ch1, ch2;
		infile >> ch1 >> ch2;
		if (ch1 == 'N'&&ch2 == 'S')
			return Token(number,ANS);
		infile.putback(ch2);
		infile.putback(ch1);
	}
	default:
		error("Bad token");
	}
}
