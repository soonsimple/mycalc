/*
 ************************************
	《PPP》 - Programming: Principles and Practice using C++ (second edition)
	已经按照第7章修改完成，
	已实现第6、7两章的功能。
	实现幂运算；
	实现阶乘运算；

	soonsimple, 12-22-2017
 ************************************
 */

#include "std_lib_facilities.h"

const char print = ';';
const string prompt = "> ";
const string result = "= ";

const char number = '8';
const char quit = 'q';
const char let = 'L';
const char name = 'a';

const string declkey = "let";

//------------------------------------------------------------------------------
class Variable
{
public:
	string name;
	double value;
	Variable(string n, double v) : name(n), value(v) {}
};

vector<Variable> var_table;

//------------------------------------------------------------------------------
double get_value(string s)
// return the value of the Variable named s
{
	for (unsigned i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s)
			return var_table[i].value;
	error("get: undefined Variable ", s);
}

//------------------------------------------------------------------------------
void set_value(string s, double d)
// set the Variable named s to d
{
	for (unsigned i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s)
		{
			var_table[i].value = d;
			return;
		}
	error("set: undefined Variable ", s);
}

//------------------------------------------------------------------------------

class Token
{
public:
	char kind;	  // what kind of token
	double value; // for numbers: a value
	string name;
	Token(char ch) // make a Token from a char
		: kind(ch), value(0)
	{
	}
	Token(char ch, double val) // make a Token from a char and a double
		: kind(ch), value(val)
	{
	}
	Token(char ch, string n) : kind(ch), name(n) {}
};

//------------------------------------------------------------------------------

class Token_stream
{
public:
	Token_stream();		   // make a Token_stream that reads from cin
	Token get();		   // get a Token (get() is defined elsewhere)
	void ignore(char c);   // discard characters up to and including c
	void putback(Token t); // put a Token back
private:
	bool full;	  // is there a Token in the buffer?
	Token buffer; // here is where we keep a Token put back using putback()
};

//------------------------------------------------------------------------------

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
	: full(false), buffer(0) // no Token in buffer
{
}

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
	if (full)
		error("putback() into a full buffer");
	buffer = t;	 // copy t to buffer
	full = true; // buffer is now full
}

//------------------------------------------------------------------------------

Token Token_stream::get()
{
	if (full)
	{ // do we already have a Token ready?
		// remove token from buffer
		full = false;
		return buffer;
	}

	char ch;
	cin >> ch; // note that >> skips whitespace (space, newline, tab, etc.)

	switch (ch)
	{
	case print:
	case '(':
	case ')':
	case '{':
	case '}':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '^':
	case '!':
	case '=':
		return Token(ch); // let each character represent itself
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		cin.putback(ch); // put digit back into the input stream
		double val;
		cin >> val; // read a floating-point number
		return Token(number, val);
	}
	default:
		if (isalpha(ch))
		{
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch)))
				s += ch;
			cin.putback(ch);
			if (s == declkey)
				return Token(let);
			if (s == "quit" || s == "q")
				return Token(quit);
			return Token(name, s);
		}
		error("Bad token");
	}
}

//------------------------------------------------------------------------------

void Token_stream::ignore(char c) // c represents the kind of Token
{
	// first look in buffer;
	if (full && c == buffer.kind)
	{
		full = false;
		return;
	}
	full = false;

	// now search input
	char ch = 0;
	while (cin >> ch)
		if (ch == c)
			return;
}

int factorial(int num)
{
	if (num < 1)
		return 1;
	else
		return num * factorial(num - 1);
}

//------------------------------------------------------------------------------

double intpower(double left, int n)
{
	if (n == 0)
		return 1;
	else
		return left * intpower(left, n - 1);
}

//------------------------------------------------------------------------------

Token_stream ts; // provides get() and putback()

//------------------------------------------------------------------------------

double expression(); // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

// deal with numbers and parentheses {}  ()
double primary()
{
	Token t = ts.get();

	switch (t.kind)
	{
	case '(': // handle '(' expression ')'
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error("')' expected");
		return d;
	}
	case '{': // handle '{' expression '}'
	{
		double d = expression();
		t = ts.get();
		if (t.kind != '}')
			error("'}' expected");
		return d;
	}

	case number:
		return t.value; // return the number's value
	case name:
		return get_value(t.name);
	case '-':
		return -primary();
	case '+':
		return primary();
	default:
		error("primary expected");
	}
}

//------------------------------------------------------------------------------

// deal with !阶乘、 ^乘方;
double pp()
{
	double left = primary();
	Token t = ts.get();

	while (true)
	{
		switch (t.kind)
		{
		case '!':
		{
			int ileft = int(left);
			left = factorial(ileft);
			t = ts.get();
			break;
		}
		case '^':
		{
			double d = pp();
			int n = int(d);
			if (n != d)
				error("accept power of integer only.");
			if (n >= 0)
				left = intpower(left, n);
			if (n < 0)
				left = 1 / intpower(left, abs(n));

			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

//------------------------------------------------------------------------------

// deal with *, /, and %
double term()
{
	double left = pp();
	Token t = ts.get(); // get the next token from token stream

	while (true)
	{
		switch (t.kind)
		{
		case '*':
			left *= pp();
			t = ts.get();
			break;
		case '/':
		{
			double d = pp();
			if (d == 0)
				error("divide by zero");
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			double d = pp();
			int i1 = int(left);
			if (i1 != left)
				error("left-hand operand of % not an integer.");
			int i2 = int(d);
			if (i2 != d)
				error("right-hand operand of % not an integer.");
			if (i2 == 0)
				error("%: divided by zero.");
			left = i1 % i2;
			t = ts.get();
			break;
		}
		default:
			ts.putback(t); // put t back into the token stream
			return left;
		}
	}
}

//------------------------------------------------------------------------------

// deal with + and -
double expression()
{
	double left = term(); // read and evaluate a Term
	Token t = ts.get();	  // get the next token from token stream

	while (true)
	{
		switch (t.kind)
		{
		case '+':
			left += term(); // evaluate Term and add
			t = ts.get();
			break;
		case '-':
			left -= term(); // evaluate Term and subtract
			t = ts.get();
			break;
		default:
			ts.putback(t); // put t back into the token stream
			return left;   // finally: no more + or -: return the answer
		}
	}
}
//------------------------------------------------------------------------------

bool is_declared(string var)
// is var already in var_table?
{
	for (unsigned i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == var)
			return true;
	return false;
}

//------------------------------------------------------------------------------
double define_name(string var, double val) // add (var, val)to var_table
{
	if (is_declared(var))
		error(var, "declared twice.");
	var_table.push_back(Variable(var, val));
	return val;
}

//------------------------------------------------------------------------------
double declaration()
{
	Token t = ts.get();
	if (t.kind != name)
		error("name expected in declaration");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=')
		error("= missing in declaration of ", var_name);
	double d = expression();
	define_name(var_name, d);
	return d;
}

//------------------------------------------------------------------------------
double statement()
{
	Token t = ts.get();
	switch (t.kind)
	{
	case let:
		return declaration();
	default:
		ts.putback(t);
		return expression();
	}
}

//------------------------------------------------------------------------------
void usage()
{
	cout << "\nWelcome to our simple calculator.\n";
	cout << "Please enter expressions using floating-point numbers.\n";
	cout << "You can define variable using syntax like this: let x1=12;\n";
	cout << "Using ';' to end your expression.\n";
	cout << "Using 'q' to exit.\n";
}

//------------------------------------------------------------------------------
void clean_up_mess()
{
	ts.ignore(print);
}

//------------------------------------------------------------------------------
void calculte()
{
	while (cin)
		try
		{
			cout << prompt;
			Token t = ts.get();
			while (t.kind == print)
				t = ts.get(); // eat all ';'
			if (t.kind == quit)
				return;
			ts.putback(t);
			cout << result << statement() << '\n';
		}
		catch (exception &e)
		{
			cerr << e.what() << endl;
			clean_up_mess();
		}
}

//------------------------------------------------------------------------------

int main()
{
	try
	{
		// predefine names:
		define_name("pi", 3.1415926535);
		define_name("e", 2.7182818284);

		usage();
		calculte();
		// keep_window_open();
		return 0;
	}
	catch (exception &e)
	{
		cerr << "error: " << e.what() << '\n';
		keep_window_open("~~");
		return 1;
	}
	catch (...)
	{
		cerr << "exception!\n";
		keep_window_open("~~");
		return 2;
	}
}

//------------------------------------------------------------------------------
