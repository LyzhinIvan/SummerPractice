#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

//Перечисление типов лексем
enum Type {ERROR, EMPTY, LETTER, DIGIT, SPACE, SEMICOLON, EQUAL, DOT, LEFTBRACKET, RIGHTBRACKET, IDENTIFIER, KEYWORD, INTNUMBER,
	PLUS, MINUS, MULTI, DIV, MOD, UNARYOPERATOR, BINARYOPERATOR, VARTYPE, OPERATION};
//Перечисление состояний конечного автомата синтаксического блока
enum State {START, KEYTYPE, TYPENAME, EQUALITY, KEYARRAY, LBRACKET, LOWBOUND, DOT1, DOT2, HIGHBOUND, RBRACKET, KEYOF, VAR, SCOLON};

//Структура, описывающая лексему-символ
struct CharToken
{
	char ch; //Значение лексемы-символа
	Type type; //Тип лексемы-символа

	//Констркутор без параметров
	CharToken()
	{
		ch = '\0';
		type = EMPTY;
	}

	//Конструктор с 2 параметрами
	CharToken(char _ch, Type _type)
	{
		ch = _ch;
		type = _type;
	}
};

//Структура, описывающая лексему-слово
struct WordToken
{
	string word; //Значение лексемы-слова
	Type type; //Тип лексемы-слова

	//Конструктор без параметров
	WordToken()
	{
		word = "";
		type = EMPTY;
	}

	//Конструктор с 2 параметрами
	WordToken(string _word, Type _type)
	{
		word = _word;
		type = _type;
	}

	//Коструктор копирования
	WordToken(CharToken ct)
	{
		word = ct.ch;
		type = ct.type;
	}
};

void solve(string); //Распознает переданную строку
void toUpperCase(string&); //Переводит строку в верхний регистр
void transliterBlock(string, vector<CharToken>&); //Блок транслитерации
void lexicalBlock(const vector<CharToken>&, vector<WordToken>&); //Лексический блок
void identifierBlock(vector<WordToken>&); //Блок идентификации ключевых слов
void simplify(vector<WordToken>&); //Процедура упрощения лексем-слов для их обработки в синтаксичском блоке
void syntaxBlock(vector<WordToken>&); //Синтаксический блок
void accept(); //Вывод сообщения о допустимой цепочке и завершение работы
void reject(); //Вывод сообщения о недопустимой цепочке и завершений работы

//Головной модуль
int main()
{
	string s = ""; //Исходная строка
	getline(cin, s); //Чтение исходной строки
	solve(s);
	system("pause");
	return 0;
}

//Распознает переданную строку
//Исходные данные: исходная символьная цепочка
//Выходные данные: вызывает либо accept(), либо reject()
void solve(string s)
{
	vector<CharToken> charTokens; //Вектор лексем-символов
	vector<WordToken> wordTokens; //Вектор лексем-слов
	toUpperCase(s); //Перевод строки в верхний регистр
	transliterBlock(s, charTokens); //Запуск блока транслитерации
	lexicalBlock(charTokens, wordTokens); //Запуск лексического блока
	identifierBlock(wordTokens); //Запуск блока идентификации ключевых слов
	syntaxBlock(wordTokens);
	accept();
}

//Переводит строку в верхний регистр
void toUpperCase(string& s)
{
	for(unsigned int i=0; i<s.size(); ++i)
	{
		if(s[i]>='a' && s[i]<='z')
			s[i] = s[i] + 'A' - 'a';
	}
}

//Блок транслитерации
//Исходные данные: исходая цепочка(тип string)
//Выходные данные: вектор лексем-символов, передающийся через параметры по ссылке
void transliterBlock(string s, vector<CharToken>& charTokens)
{
	charTokens.clear();
	for(unsigned int i=0; i<s.size(); ++i)
	{
		Type type;
		if(s[i]>='A' && s[i]<='Z')
			type = LETTER;
		else if(s[i]>='0' && s[i]<='9')
			type = DIGIT;
		else switch(s[i])
		{
		case ';':
			type = SEMICOLON;
			break;
		case ' ':
			type = SPACE;
			break;
		case '.':
			type = DOT;
			break;	
		case '=':
			type = EQUAL;
			break;
		case '[':
			type = LEFTBRACKET;
			break;
		case ']':
			type = RIGHTBRACKET;
			break;
		case '+':
			type = PLUS;
			break;
		case '-':
			type = MINUS;
			break;
		case '*':
			type = MULTI;
			break;
		default:
			reject();
		}
		charTokens.push_back(CharToken(s[i], type));
	}
}

//Лексический блок
//Исходные данные: вектор лексем-символов, передающийся по константной ссылке
//Выходные данные: вектор лексем-слов, передающийся через параметры по ссылке
void lexicalBlock(const vector<CharToken> &charTokens, vector<WordToken> &wordTokens)
{
	wordTokens.clear();
	string curWord = "";
	Type curType = EMPTY;
	for(size_t i=0; i<charTokens.size(); ++i)
	{
		switch(curType)
		{
		case EMPTY:
		case IDENTIFIER:
			switch(charTokens[i].type)
			{
			case LETTER:
				curWord += charTokens[i].ch;
				curType = IDENTIFIER;
				break;
			case DIGIT:
				curWord += charTokens[i].ch;
				if(curType==EMPTY)
					curType = INTNUMBER;
				break;
			case SPACE:
			case EQUAL:
			case SEMICOLON:
			case LEFTBRACKET:
			case RIGHTBRACKET:
			case PLUS:
			case MINUS:
			case MULTI:
			case DOT:
				if(curType==IDENTIFIER)
					wordTokens.push_back(WordToken(curWord, curType));
				if(charTokens[i].type!=SPACE)
					wordTokens.push_back(WordToken(charTokens[i]));
				curWord="";
				curType = EMPTY;
				break;
			default:
				reject();
			}
			break;
		case INTNUMBER:
			switch(charTokens[i].type)
			{
			case LETTER:
				reject();
				break;
			case DIGIT:
				curWord += charTokens[i].ch;
				break;
			case SPACE:
			case EQUAL:
			case SEMICOLON:
			case LEFTBRACKET:
			case RIGHTBRACKET:
			case DOT:
			case PLUS:
			case MINUS:
			case MULTI:
				wordTokens.push_back(WordToken(curWord, curType));
				if(charTokens[i].type!=SPACE)
					wordTokens.push_back(charTokens[i]);
				curWord="";
				curType=EMPTY;
				break;
			default:
				reject();
			}
			break;
		}
	}
	if(curType!=EMPTY)
		wordTokens.push_back(WordToken(curWord, curType));
}

//Блок идентификации ключевых слов
//Исходные данные: вектор лексем-слов, переданный по ссылке
//Выходные данные: вектор лексем-слов с распознынными ключевыми словами, изменяется исходный вектор
void identifierBlock(vector<WordToken>& wordTokens)
{
	vector<string> keyWords;
	ifstream in("keywords.txt");
	string cur;
	while(!in.eof())
	{
		in>>cur;
		keyWords.push_back(cur);
	}
	in.close();
	vector<string> varTypes;
	in.open("vartypes.txt");
	while(!in.eof())
	{
		in>>cur;
		varTypes.push_back(cur);
	}
	in.close();
	for(size_t i=0; i<wordTokens.size(); ++i)
	{
		for(size_t j=0; j<keyWords.size(); ++j)
			if(wordTokens[i].word == keyWords[j])
				wordTokens[i].type = KEYWORD;
		for(size_t j=0; j<varTypes.size(); ++j)
			if(wordTokens[i].word == varTypes[j])
				wordTokens[i].type = VARTYPE;
		if(wordTokens[i].word=="DIV")
			wordTokens[i].type = DIV;
		if(wordTokens[i].word=="MOD")
			wordTokens[i].type = MOD;

	}
}

//Процедура упрощения лексем-слов для их обработки в синтаксичском блоке
//Исходные данные: вектор лексем-слов, переданный по ссылке
//Выходные данные: тот же вектор лексем-слов, но с упрощенным набором типов лексем
void simplify(vector<WordToken>& wordTokens)
{
	for(size_t i=0; i<wordTokens.size(); ++i)
	{
		if((wordTokens[i].type==MULTI || wordTokens[i].type==DIV || wordTokens[i].type==MOD) && 
			i>0 && wordTokens[i-1].type==INTNUMBER && i+1<wordTokens.size() && wordTokens[i+1].type==IDENTIFIER)
		{
			wordTokens[i].type=OPERATION;
			wordTokens.erase(wordTokens.begin()+i+1);
			wordTokens.erase(wordTokens.begin()+i-1);
			i -= 2;
		}
		else if(wordTokens[i].type==PLUS || wordTokens[i].type==MINUS)
		{
			if(i>0 && wordTokens[i-1].type==INTNUMBER && i+1<wordTokens.size() && wordTokens[i+1].type==IDENTIFIER)
			{
				wordTokens[i].type=OPERATION;
				wordTokens.erase(wordTokens.begin()+i+1);
				wordTokens.erase(wordTokens.begin()+i-1);
				i -= 2;
			}
			else if(i+1<wordTokens.size() && wordTokens[i+1].type==INTNUMBER)
			{
				wordTokens.erase(wordTokens.begin()+i);
				i--;
			}
		}
	}
}

//Синтаксический блок
//Исходные данные: вектор лексем-слов, переданный по ссылке
//Выходные данные: если цепочка лексем допустима, то функция просто завершает свою работу, иначе вызывает reject()
void syntaxBlock(vector<WordToken>& wordTokens)
{
	simplify(wordTokens);
	State curState = START;
	for(size_t i=0; i<wordTokens.size(); ++i)
	{
		switch(wordTokens[i].type)
		{
		case KEYWORD:
			if(wordTokens[i].word=="TYPE")
				if(curState==START)
					curState=KEYTYPE;
				else reject();
			else if(wordTokens[i].word=="ARRAY")
				if(curState==EQUALITY)
					curState=KEYARRAY;
				else reject();
			else if(wordTokens[i].word=="OF")
				if(curState==RBRACKET)
					curState=KEYOF;
				else reject();
			else reject();
			break;
		case IDENTIFIER:
			if(curState==KEYTYPE)
				curState=TYPENAME;
			else reject();
			break;
		case EQUAL:
			if(curState==TYPENAME)
				curState=EQUALITY;
			else reject();
			break;
		case LEFTBRACKET:
			if(curState==KEYARRAY)
				curState=LBRACKET;
			else reject();
			break;
		case RIGHTBRACKET:
			if(curState==HIGHBOUND)
				curState=RBRACKET;
			else reject();
			break;
		case DOT:
			if(curState==LOWBOUND)
				curState=DOT1;
			else if(curState==DOT1)
				curState=DOT2;
			else reject();
			break;
		case OPERATION:
			if(curState==LBRACKET)
				curState=LOWBOUND;
			else if(curState==DOT2)
				curState=HIGHBOUND;
			else reject();
			break;
		case VARTYPE:
			if(curState==KEYOF)
				curState=VAR;
			else reject();
			break;
		case SEMICOLON:
			if(curState==VAR)
				curState=SCOLON;
			else reject();
			break;
		default:
			reject();
		}
	}
	if(curState!=SCOLON) reject();
}

//Вывод сообщения о допустимой цепочке и завершение работы
void accept()
{
	cout<<"ACCEPT"<<endl;
	system("pause");
	exit(0);
}

//Вывод сообщения о недопустимой цепочке и завершение работы
void reject()
{
	cout<<"REJECT"<<endl;
	system("pause");
	exit(0);
}
