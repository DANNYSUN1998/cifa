#include<stack>
#include<string>
#include<iostream>
#include<vector>

using namespace std;
//用e代表空，i代表num
class NonTerminal
{
public:
	char C;              //非终结符
	vector<char> first;  //非终结符first集
	vector<char> follow; //非终结符follow集

	NonTerminal(char ch, string First, string Follow)//构造函数
	{
		C = ch;
		int i = 0;
		while (First[i] != '\0')
		{
			first.push_back(First[i]);
			i++;
		}
		i = 0;
		while (Follow[i] != '\0')
		{
			follow.push_back(Follow[i]);
			i++;
		}
	}
	int First_Empty()  //判断First是否含空
	{
		for (auto u : first)
		{
			if (u == 'e')
				return 1;
		}
		return 0;
	}
};

class Production {
public:
	char noend;                      //产生式左部
	string pro;                      //产生式右部
	vector<char> first;              //产生式右部First集
	vector<char>noend_follow;        //产生式左部的follow集
	Production(char CH, string P)    //构造函数
	{
		noend = CH;
		pro = P;
	}

	void addFirst(char ch)           //将ch加入first集
	{
		first.push_back(ch);
	}

	void add_noend_follow(char ch)   //将ch加入follow集
	{
		noend_follow.push_back(ch);

	}

	int isinFirst(char ch)           //判断ch是否在first集中
	{
		for (auto u : first)
		{
			if (u == ch)
				return 1;
		}
		return 0;
	}

	int First_Empty()                //判断first集是否含空
	{
		for (auto u : first)
		{
			if (u == 'e')
				return 1;
		}
		return 0;
	}
};

class Analyse_Sheet {
public:
	char noend;                   //左部
	char end;                     //终结符
	string pro;                   //右部产生式

	Analyse_Sheet(char Nonterminal, char Terminal, string p) :noend(Nonterminal), end(Terminal), pro(p) {};//构造函数

};

vector<NonTerminal> NT;        //非终结符容器

vector<Production> Pro;        //单生成式容器

vector<Analyse_Sheet> AS;      //分析表

vector<char> VT = { '(' , ')','*','/','+','-','$','i' };               //终结符容器

string input;                    //输入串

stack<char> AnalyseStack;      //分析栈

int isTerminal(char ch)        //判断ch是否是终结符
{
	for (auto u : NT)
	{
		if (u.C == ch)
			return 1;
	}
	return 0;
}

int Check_Analyse_Sheet(char nonterminal, char terminal, string p)   //查看分析表中是否有对应项
{
	for (auto u : AS)
	{
		if (u.noend == nonterminal && u.end == terminal && u.pro == p)
			return 1;
	}
	return 0;
}

void Init_NT()                //初始化非终结符容器
{
	NT.push_back(NonTerminal('E', "(i", "$)"));
	NT.push_back(NonTerminal('X', "+-e", "$)"));
	NT.push_back(NonTerminal('T', "(i", "$)+-"));
	NT.push_back(NonTerminal('Y', "*/e", "$)+-"));
	NT.push_back(NonTerminal('F', "(i", "+-*/)$"));
}

void Init_Pro()               //初始化产生式容器及右部First集
{
	Pro.push_back(Production('E', "TX"));
	Pro.push_back(Production('X', "+TX"));
	Pro.push_back(Production('X', "-TX"));
	Pro.push_back(Production('X', "e"));
	Pro.push_back(Production('T', "FY"));
	Pro.push_back(Production('Y', "*FY"));
	Pro.push_back(Production('Y', "/FY"));
	Pro.push_back(Production('Y', "e"));
	Pro.push_back(Production('F', "(E)"));
	Pro.push_back(Production('F', "i"));

	//构造产生式右部的first集
	for (int j = 0; j < 10; j++)
	{
		if (Pro[j].pro == "e")
		{
			Pro[j].first.push_back('e');
		}
		else
		{
			int i = 0;
			int quit;
			while (Pro[j].pro[i] != '\0')
			{
				quit = 1;
				if (isTerminal(Pro[j].pro[i]))
				{
					for (auto u : NT)
					{
						if (u.C == Pro[j].pro[i])
						{
							for (auto a : u.first)
							{
								if (!Pro[j].isinFirst(a) && a != 'e')//a还未添加到first集中且不为空
								{
									Pro[j].addFirst(a);
								}

							}
							if (u.First_Empty())
								quit = 0;
						}
					}
				}
				else
				{
					Pro[j].addFirst(Pro[j].pro[i]);
				}
				if (quit == 1)
					break;
				i++;
			}
			if (quit == 0)
			{
				Pro[j].addFirst('e');
			}
		}
	}

	for (int j = 0; j < 10; j++)
	{
		for (auto u : NT)
		{
			if (u.C == Pro[j].noend)
			{
				for (auto a : u.follow)
					Pro[j].noend_follow.push_back(a);
			}
		}
	}

}

void Init_AS()    //构造分析表
{
	for (auto u : Pro)
	{
		if (u.First_Empty())//生成式First集中包含空
		{
			for (auto a : u.first)//对所有除空以外终结符，添加生成式
			{

				if (a != 'e')
				{
					AS.push_back(Analyse_Sheet(u.noend, a, u.pro));
				}
			}
			for (auto a : u.noend_follow)//对所有左部Follow集中终结符，添加生成式
			{
				if (!Check_Analyse_Sheet(u.noend, a, u.pro))
				{
					AS.push_back(Analyse_Sheet(u.noend, a, u.pro));
				}

			}

		}
		else//生成式First中不含空
		{
			for (auto a : u.first)
				AS.push_back(Analyse_Sheet(u.noend, a, u.pro));
		}
	}
}

string Analyse_Sheet_Reference(char NonTerminal, char Terminal)//查分析表返回动作
{
	int key = 0;
	for (auto u : AS)
	{
		if (u.end == Terminal && u.noend == NonTerminal)
		{
			key = 1;
			return u.pro;
		}
	}
	if (key == 0)
	{
		cout << "未找到该动作！" << endl;
		exit(3);//查表失败
	}
}

void Stack_Print()      //自底向上打印栈函数
{
	stack<char> tmp1(AnalyseStack);
	stack<char> tmp2;
	while (tmp1.size() != 0)
	{
		char ch = tmp1.top();
		tmp1.pop();
		tmp2.push(ch);
	}

	while (tmp2.size() != 0)
	{
		cout << tmp2.top() << " ";
		tmp2.pop();
	}
}

void Input_Print(int start)  //打印输入栈
{
	for (int i = start; input[i] != '\0'; i++)
		cout << input[i] << " ";
}

void Push_Stack(string str)  //将str压入分析栈
{
	int i = str.length() - 1;
	if (i == 0)
	{
		AnalyseStack.push(str[i]);
	}
	else
	{
		while (i != 0)
		{
			AnalyseStack.push(str[i]);
			i--;
		}
		AnalyseStack.push(str[i]);
	}
}

int CheckAnalyseSheet(char NonTerminal, char Terminal)//查找分析表对应表项并打印
{
	for (auto u : AS)
	{
		if (u.end == Terminal && u.noend == NonTerminal)
		{
			cout << u.noend << "->" << u.pro << "(" << u.end << ")" << '	';
			return 1;
		}
	}
	return 0;
}

void PrintAnalyseSheet()      //打印分析表
{
	for (auto u : NT)
	{
		for (auto v : VT)
		{
			CheckAnalyseSheet(u.C, v);
		}
		cout << endl;
	}
}

void Analyse_Process()     //分析过程
{
	cout << "Please input the string" << endl;
	getline(cin, input);
	input.push_back('$');                     //在输入的符号串末尾加入$

	AnalyseStack.push('$');
	AnalyseStack.push('E'); //将$和E开始非终结符压入分析栈

	int index = 0;   //input输入串的跑标
	int step = 1;
	cout << "步骤" << "		分析栈	  " << "		输入符号串	" << "				动作	" << endl;
	while (!(AnalyseStack.top() == '$' && (input[index] == '$')))
	{
		if (input[index] == ' ')
		{
			index++;
			continue;
		}
		cout << step << "	";
		step++;
		Stack_Print();                     //打印分析栈
		cout << "			";
		Input_Print(index);                //打印输入栈
		cout << "				";
		if (isTerminal(AnalyseStack.top()) == 0) //分析栈顶是终结符
		{
			if (AnalyseStack.top() == input[index])//符号栈与输入栈栈顶元素匹配，弹出
			{
				AnalyseStack.pop();
				index++;
				cout << "匹配" << endl;
			}
			else                                //不匹配，error
			{
				cout << "匹配错误！" << endl;
				exit(2);
			}
		}
		else
		{
			string tmp = Analyse_Sheet_Reference(AnalyseStack.top(), input[index]);
			cout << AnalyseStack.top() << " -> " << tmp << endl;
			if (tmp == "e")                        //A->空
			{
				AnalyseStack.pop();
			}
			else
			{
				AnalyseStack.pop();
				Push_Stack(tmp);
			}
		}
		if (AnalyseStack.top() == '$' && (input[index] == '$'))
		{
			cout << step << "	";
			step++;
			Stack_Print();                     //打印分析栈
			cout << "			";
			Input_Print(index);                //打印输入栈
			cout << "				";
			cout << "完成" << endl;
		}
	}
}

int main()
{
	Init_NT();
	Init_Pro();
	Init_AS();
	PrintAnalyseSheet();
	Analyse_Process();

	system("pause");
	return 0;
}

