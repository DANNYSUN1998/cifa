#include<stack>
#include<string>
#include<iostream>
#include<vector>

using namespace std;
//��e����գ�i����num
class NonTerminal
{
public:
	char C;              //���ս��
	vector<char> first;  //���ս��first��
	vector<char> follow; //���ս��follow��

	NonTerminal(char ch, string First, string Follow)//���캯��
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
	int First_Empty()  //�ж�First�Ƿ񺬿�
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
	char noend;                      //����ʽ��
	string pro;                      //����ʽ�Ҳ�
	vector<char> first;              //����ʽ�Ҳ�First��
	vector<char>noend_follow;        //����ʽ�󲿵�follow��
	Production(char CH, string P)    //���캯��
	{
		noend = CH;
		pro = P;
	}

	void addFirst(char ch)           //��ch����first��
	{
		first.push_back(ch);
	}

	void add_noend_follow(char ch)   //��ch����follow��
	{
		noend_follow.push_back(ch);

	}

	int isinFirst(char ch)           //�ж�ch�Ƿ���first����
	{
		for (auto u : first)
		{
			if (u == ch)
				return 1;
		}
		return 0;
	}

	int First_Empty()                //�ж�first���Ƿ񺬿�
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
	char noend;                   //��
	char end;                     //�ս��
	string pro;                   //�Ҳ�����ʽ

	Analyse_Sheet(char Nonterminal, char Terminal, string p) :noend(Nonterminal), end(Terminal), pro(p) {};//���캯��

};

vector<NonTerminal> NT;        //���ս������

vector<Production> Pro;        //������ʽ����

vector<Analyse_Sheet> AS;      //������

vector<char> VT = { '(' , ')','*','/','+','-','$','i' };               //�ս������

string input;                    //���봮

stack<char> AnalyseStack;      //����ջ

int isTerminal(char ch)        //�ж�ch�Ƿ����ս��
{
	for (auto u : NT)
	{
		if (u.C == ch)
			return 1;
	}
	return 0;
}

int Check_Analyse_Sheet(char nonterminal, char terminal, string p)   //�鿴���������Ƿ��ж�Ӧ��
{
	for (auto u : AS)
	{
		if (u.noend == nonterminal && u.end == terminal && u.pro == p)
			return 1;
	}
	return 0;
}

void Init_NT()                //��ʼ�����ս������
{
	NT.push_back(NonTerminal('E', "(i", "$)"));
	NT.push_back(NonTerminal('X', "+-e", "$)"));
	NT.push_back(NonTerminal('T', "(i", "$)+-"));
	NT.push_back(NonTerminal('Y', "*/e", "$)+-"));
	NT.push_back(NonTerminal('F', "(i", "+-*/)$"));
}

void Init_Pro()               //��ʼ������ʽ�������Ҳ�First��
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

	//�������ʽ�Ҳ���first��
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
								if (!Pro[j].isinFirst(a) && a != 'e')//a��δ��ӵ�first�����Ҳ�Ϊ��
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

void Init_AS()    //���������
{
	for (auto u : Pro)
	{
		if (u.First_Empty())//����ʽFirst���а�����
		{
			for (auto a : u.first)//�����г��������ս�����������ʽ
			{

				if (a != 'e')
				{
					AS.push_back(Analyse_Sheet(u.noend, a, u.pro));
				}
			}
			for (auto a : u.noend_follow)//��������Follow�����ս�����������ʽ
			{
				if (!Check_Analyse_Sheet(u.noend, a, u.pro))
				{
					AS.push_back(Analyse_Sheet(u.noend, a, u.pro));
				}

			}

		}
		else//����ʽFirst�в�����
		{
			for (auto a : u.first)
				AS.push_back(Analyse_Sheet(u.noend, a, u.pro));
		}
	}
}

string Analyse_Sheet_Reference(char NonTerminal, char Terminal)//��������ض���
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
		cout << "δ�ҵ��ö�����" << endl;
		exit(3);//���ʧ��
	}
}

void Stack_Print()      //�Ե����ϴ�ӡջ����
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

void Input_Print(int start)  //��ӡ����ջ
{
	for (int i = start; input[i] != '\0'; i++)
		cout << input[i] << " ";
}

void Push_Stack(string str)  //��strѹ�����ջ
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

int CheckAnalyseSheet(char NonTerminal, char Terminal)//���ҷ������Ӧ�����ӡ
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

void PrintAnalyseSheet()      //��ӡ������
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

void Analyse_Process()     //��������
{
	cout << "Please input the string" << endl;
	getline(cin, input);
	input.push_back('$');                     //������ķ��Ŵ�ĩβ����$

	AnalyseStack.push('$');
	AnalyseStack.push('E'); //��$��E��ʼ���ս��ѹ�����ջ

	int index = 0;   //input���봮���ܱ�
	int step = 1;
	cout << "����" << "		����ջ	  " << "		������Ŵ�	" << "				����	" << endl;
	while (!(AnalyseStack.top() == '$' && (input[index] == '$')))
	{
		if (input[index] == ' ')
		{
			index++;
			continue;
		}
		cout << step << "	";
		step++;
		Stack_Print();                     //��ӡ����ջ
		cout << "			";
		Input_Print(index);                //��ӡ����ջ
		cout << "				";
		if (isTerminal(AnalyseStack.top()) == 0) //����ջ�����ս��
		{
			if (AnalyseStack.top() == input[index])//����ջ������ջջ��Ԫ��ƥ�䣬����
			{
				AnalyseStack.pop();
				index++;
				cout << "ƥ��" << endl;
			}
			else                                //��ƥ�䣬error
			{
				cout << "ƥ�����" << endl;
				exit(2);
			}
		}
		else
		{
			string tmp = Analyse_Sheet_Reference(AnalyseStack.top(), input[index]);
			cout << AnalyseStack.top() << " -> " << tmp << endl;
			if (tmp == "e")                        //A->��
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
			Stack_Print();                     //��ӡ����ջ
			cout << "			";
			Input_Print(index);                //��ӡ����ջ
			cout << "				";
			cout << "���" << endl;
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

