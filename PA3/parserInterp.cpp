/* Implementation of Recursive-Descent Parser
 * for the SFort95 Language
 * parser(SP24).cpp
 * Programming Assignment 2
 * Spring 2024
*/
//#include <string> 
#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value>* ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//don't have to change prog
//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = Decl(in, line);
			if (!dl)
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if (!sl)
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == IDENT) {
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}
//don't have to change
//Decl ::= Type :: VarList 
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)] 
bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	string strLen;

	LexItem t = Parser::GetNextToken(in, line);

	if (t == INTEGER || t == REAL || t == CHARACTER) {
		tok = t;

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON) {
			status = VarList(in, line, t);

			if (status)
			{
				status = Decl(in, line);
				if (!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if (t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == ICONST)
					{
						strLen = tok.GetLexeme();

						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if (tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, t, stoi(strLen));

								if (status)
								{
									//cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
									status = Decl(in, line);
									if (!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}

					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}

	}

	Parser::PushBackToken(t);
	return true;
}//End of Decl

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case PRINT:
		status = PrintStmt(in, line);

		if (status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;


	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of Stmt

bool SimpleStmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case PRINT:
		status = PrintStmt(in, line);

		if (!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}
		cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		cout << "Assignment statement in a Simple If statement." << endl;

		break;


	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of SimpleStmt

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem& idtok, int strlen) {
	bool status = false, exprstatus = false;
	string identstr;
	Value returnValue;


	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		//gets the lexeme
		identstr = tok.GetLexeme();
		if (idtok == CHARACTER) {
			//add to SymTable to save string and token
			SymTable[identstr] = idtok.GetToken();
			string strHold(strlen, ' ');//will take string lenght and add the correct amount of space assigned
			returnValue = Value(strHold);
			TempsResults[tok.GetLexeme()] = returnValue;//add to TempsResults
			defVar[tok.GetLexeme()] = true;// change it to true to show that it's been declared

		}
		else {
			defVar[tok.GetLexeme()] = false;// this mean it has not been declared

		}


	}
	else
	{

		ParseError(line, "Missing Variable Name");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok == ASSOP)
	{

		exprstatus = Expr(in, line, returnValue);
		if (!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		
		SymTable[identstr] = idtok.GetToken();


		if (idtok != CHARACTER) {
			if (!(defVar.find(identstr)->second))
			{
				defVar[identstr] = true;
			}
			else
			{
				ParseError(line, "Variable Redefinition");
				return false;
			}
		}

		if (idtok == CHARACTER) {
			//case where Value is smaller then length
			if (returnValue.GetString().length() > strlen) {
				string temp = returnValue.GetString().substr(0, strlen);
				TempsResults[identstr] = Value(temp);//change TempsResults
			}
			else if (returnValue.GetString().length() < strlen) {
				int numSpacesToAdd = strlen - returnValue.GetString().length();//calculates spacedToAdd

				string space(numSpacesToAdd, ' ');
				string temp = returnValue.GetString() + space;
				TempsResults[identstr] = Value(temp);

			}
			else {
				TempsResults[identstr] = returnValue;
			}
		}
		else if (idtok == REAL) {
			//if supposed to be an int but is real converts to real and changes in TempsResults
			if (returnValue.IsInt()) {
				double newRealNum = returnValue.GetInt();//chnages int into a double
				TempsResults[identstr] = Value(newRealNum);//changes value in TempsResults
			}
		}
		else {
			TempsResults[identstr] = returnValue;
		}
		
		
		//cout<< "Initialization of the variable " << identstr <<" in the declaration statement." << endl;
		tok = Parser::GetNextToken(in, line);

		if (tok == COMMA) {

			status = VarList(in, line, idtok, strlen);

		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA) {

		status = VarList(in, line, idtok, strlen);
	}
	else if (tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");

		return false;
	}
	else {

		Parser::PushBackToken(tok);
		return true;
	}

	return status;

}//End of VarList



//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;


	t = Parser::GetNextToken(in, line);

	if (t != DEF) {

		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	if (t != COMMA) {

		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}

	while (!(*ValQue).empty())
	{
		Value nextValue = (*ValQue).front();
		cout << nextValue;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of PrintStmt

//BlockIfStmt:= if (Expr) then {Stmt} [Else Stmt]
//SimpleIfStatement := if (Expr) Stmt
bool BlockIfStmt(istream& in, int& line) {
	bool ex = false, status;
	static int nestlevel = 0;
	//int level; don't need this
	LexItem t;
	Value returnValue;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	/*
	commented out because don't need it
	if (returnValue.GetType() == VERR) {
		ParseError(line, "Missing this opperand in BlockIf");
		return false;
	}
	*/
	ex = RelExpr(in, line, returnValue);
	if (!ex) {
		ParseError(line, "Missing if statement condition");
		return false;
	}
	if (returnValue.IsBool() && !returnValue.GetBool()) {
		
		while (t != END && t != ELSE) {
			t = Parser::GetNextToken(in, line);
		}
		Parser::PushBackToken(t);
	
	}
	else {

		t = Parser::GetNextToken(in, line);
		if (t != RPAREN) {

			ParseError(line, "Missing Right Parenthesis");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if (t != THEN)
		{
			Parser::PushBackToken(t);

			status = SimpleStmt(in, line);
			if (status)
			{
				return true;
			}
			else
			{
				ParseError(line, "If-Stmt Syntax Error");
				return false;
			}


		}
		nestlevel++;
		//level = nestlevel; dont need this
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
	}
	t = Parser::GetNextToken(in, line);
	if (t == ELSE) {
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		else
			t = Parser::GetNextToken(in, line);

	}


	if (t != END) {

		ParseError(line, "Missing END of IF");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t == IF) {
		//cout << "End of Block If statement at nesting level " << level << endl; dont need this
		return true;
	}

	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem& idtok)
{
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT) {
		idtok = tok;
		identstr = tok.GetLexeme();

		
		if (!(defVar.find(identstr)->second))
		{
			/*
			dont' need this
			ParseError(line, "Undeclared Variable");
			return false;*/

			defVar[identstr] = true;
		}
		
		return true;
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {

	bool varstatus = false, status = false;
	LexItem t;
	Value returnValue;
	//int holderLine=line; added because I thought we needed to save line number but we dont

	LexItem holdLex;
	varstatus = Var(in, line, holdLex);
	//cout << holdLex << endl;



	if (varstatus) {
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP) {
			status = Expr(in, line, returnValue);
			
			if (!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}

			
		
			

			if (SymTable[holdLex.GetLexeme()] == CHARACTER) {

				if (!returnValue.IsString()) {
					ParseError(line-2, "Illegal mixed-mode assignment operation");
					return false;
				}
				//cout << holdLex << endl; use to debug
				int strlen = TempsResults[holdLex.GetLexeme()].GetString().length();
				//case where Value is smaller then length
				if (returnValue.GetString().length() > strlen) {
					string temp = returnValue.GetString().substr(0, strlen);
					TempsResults[holdLex.GetLexeme()] = Value(temp);
				}
				else if (returnValue.GetString().length() < strlen) {
					int numSpacesToAdd = strlen - returnValue.GetString().length();

					string space(numSpacesToAdd, ' ');
					string temp = returnValue.GetString() + space;
					TempsResults[holdLex.GetLexeme()] = Value(temp);

				}
				else {
					TempsResults[holdLex.GetLexeme()] = returnValue;
				}
			}
			else {
				TempsResults[holdLex.GetLexeme()] = returnValue;
			}

			





		}
		else if (t.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator, fix");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement, have to fix this");
		return false;
	}
	return status;
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);//add tos ValQue
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {

		status = ExprList(in, line);

	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value& retVal) {

	bool t1 = Expr(in, line, retVal);
	LexItem tok;
	Value returnValue;
	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == EQ || tok == LTHAN || tok == GTHAN)
	{
		t1 = Expr(in, line, returnValue);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == GTHAN) {
			retVal = retVal > returnValue;//perform > from val.cpp
		}
		else if (tok == LTHAN) {
			retVal = retVal < returnValue;//perform < from val.cpp
		}
		else {
			retVal = retVal == returnValue;//perform == from val.cpp
		}

	}

	if (retVal.IsErr()) {
		ParseError(line-2, "Illegal operand types for a Relational operation");
		return false;
	}

	return true;
}//End of RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value& retVal) {
	Value returnValue;
	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == PLUS || tok == MINUS || tok == CAT)
	{
		t1 = MultExpr(in, line, returnValue);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == MINUS) {
			retVal = retVal - returnValue;//perform - from val.cpp
		}
		else if (tok == PLUS) {
			retVal = retVal + returnValue;// perform + from val.cpp
		}
		else if (tok == CAT) {
			retVal = retVal.Catenate(returnValue);// perform Catenate from val.cpp
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}

	if (retVal.IsErr()) {
		ParseError(line-2, "Illegal operand type for the operation");
		return false;
	}

	Parser::PushBackToken(tok);
	return true;
}//End of Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value& retVal) {

	bool t1 = TermExpr(in, line, retVal);
	LexItem tok;
	Value returnValue;
	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV)
	{
		t1 = TermExpr(in, line, returnValue);

		if (!t1) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == MULT) {
			retVal = retVal * returnValue;
		}
		else if (tok == DIV) {
			if(returnValue.IsInt()){
				if(returnValue.GetInt()==0){
					ParseError(line-1,"Cannot divide by 0, ILLEGAL DIVISION ERRORERRORERROR");
					return false;
					}
			}
			if(returnValue.IsReal()){
				if(returnValue.GetReal()==0){
					ParseError(line-1,"Cannot divide by 0, ILLEGAL DIVISION ERRORERRORERROR");
					return false;
					}
			}
			retVal = retVal / returnValue;//after checking if the returnValue type(int/real) then will check if the value is 0. Once done it will continue to divide
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}

	if (retVal.IsErr()) {
		ParseError(line, "Illegal operand type for the operation");
		return false;
	}

	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value& retVal) {
	bool t1 = SFactor(in, line, retVal);
	LexItem tok;
	queue<Value> powerQueue;

	if (!t1) {
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	//powerQueue.push(retVal);
	while (tok == POW)
	{
		Value returnValue;
		t1 = SFactor(in, line, returnValue);

		if (!t1) {
			ParseError(line, "Missing exponent operand");
			return false;
		}
		powerQueue.push(returnValue);
		//powerQueue.pop(); this will mess everything up because we just added something, so we shouldn't pop
		while(powerQueue.size()!=0){ //check  powerQueue size to make sure it's not empty or everything will break
			if (tok == POW&& powerQueue.size()!=0) {//will check token and once again check powerQueue just in case
				returnValue=powerQueue.front();//will get value from front of queue
				powerQueue.pop();//removes value from front of queue
				retVal =retVal.Power(returnValue);// will perform power from val.cpp
				}
		}
		retVal=retVal*5;
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	if (retVal.IsErr()) {
		ParseError(line-2, "Illegal operand type for the operation");
		return false;
	}
	Parser::PushBackToken(tok);
	return true;
}//End of TermExpr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value& retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if (t == MINUS)//check sign and properly makes it
	{
		sign = -1;//makes it negative
	}
	else if (t == PLUS)
	{
		sign = 1;//makes it positive
	}
	else
		Parser::PushBackToken(t);

	status = Factor(in, line, sign, retVal);
	return status;
}//End of SFactor

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value& retVal) {
	Value returnValue;

	LexItem tok = Parser::GetNextToken(in, line);
	//cout<<tok<<endl; used to test
	//cout << tok.GetLexeme() << endl; used to test
	if (tok == IDENT) {

		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		returnValue = TempsResults[lexeme];
		if (sign == -1 || sign == 1) { 
			if (returnValue.IsString()) {//check sign and will make sure that if there is a negative then it will throw an error
				ParseError(line, "String can't be negative, SOMETHING IS VERY WRONG");
				return false;
			}
			if (returnValue.IsInt()) {
				returnValue = Value( sign * returnValue.GetInt());
			}
			if (returnValue.IsReal()) {
				returnValue = Value( sign * returnValue.GetReal());
			}
		}
		retVal = returnValue;
		return true;
	}
	//left off here
	else if (tok == ICONST) {
		if (sign == -1) {//check sign
			returnValue = Value(-stoi(tok.GetLexeme()));//convert to negative
		}
		else {
			returnValue = Value(stoi(tok.GetLexeme()));//keeps positive if not -1
		}
		retVal = returnValue;
		return true;
	}
	else if (tok == SCONST) {
		returnValue = Value(tok.GetLexeme());
		retVal = returnValue;
		return true;
	}
	else if (tok == RCONST) {
		if (sign == -1) {//check sign
			returnValue = Value(stod(tok.GetLexeme()));
		}
		else {
			returnValue = Value(stod(tok.GetLexeme()));
		}
		//retVal=returnValue;
		retVal = returnValue;
		return true;
	}
	else if (tok == LPAREN) {
		bool ex = Expr(in, line, returnValue);
		if (!ex) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		retVal = returnValue;
		if (Parser::GetNextToken(in, line) == RPAREN)
			return ex;
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}


	return false;
}