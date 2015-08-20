#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <vector>
using std::vector;
#include <string>
using std::string;
using std::getline;
#include <fstream>
using std::ifstream;

struct Token {
	string *source;
	size_t start, end;
};

struct Identifier : Token {
	// verify meets identifier rules
};

enum class AtomType { Invalid, Import, Type, Function, Declaration, Statement, Literal };
struct Atom {
	AtomType type;
	Token source;
};

struct ImportWhich {
	Token which;

	Token *comma;
	ImportWhich *next;
};
struct Import : Atom {
	Token keyword;
	ImportWhich which;
	Token semicolon;
};

struct Statement {};
struct DeclarationPiece {
	Token name;
	Token *equals;
	Statement *statement;

	Token *comma;
	DeclarationPiece *next;
};
struct Declaration : Atom {
	Token type;
	DeclarationPiece name;
	Token semicolon;
};

struct GenericList {
};
struct ArgumentList {
};
struct Function : Atom {
	Token keyword;
	Identifier id;

};

struct Scope {
};

vector<string> tokenize(string text);
vector<string> tokenize(string text) {
	vector<string> tokens;
	size_t idx = 0;
	while(idx < text.size()) {
		//if(
	}
	return tokens;
}

struct Parser {
	virtual ~Parser() { }
	virtual bool parse(string &str, size_t &idx) = 0;
	bool parse(string str) {
		size_t idx = 0;
		bool s = parse(str, idx);
		cout << "success: " << s << endl;
		cout << "idx: " << idx << endl;
		return s;
	}
};

struct AndCombinator : public Parser {
	AndCombinator(Parser &p1, Parser &p2) : _p1(&p1), _p2(&p2) { }
	bool parse(string &str, size_t &idx) {
		size_t i_idx = idx;
		bool p1Success = _p1->parse(str, idx);
		if(!p1Success) {
			idx = i_idx;
			return false;
		}
		bool p2Success = _p2->parse(str, idx);
		if(!p2Success) {
			idx = i_idx;
			return false;
		}
		return true;
	}

	protected:
		Parser *_p1;
		Parser *_p2;
};
struct OrCombinator : public Parser {
	OrCombinator(Parser &p1, Parser &p2) : _p1(&p1), _p2(&p2) { }
	bool parse(string &str, size_t &idx) {
		size_t i_idx = idx;
		bool p1Success = _p1->parse(str, idx);
		if(p1Success) { return true; }
		idx = i_idx; // rewind
		bool p2Success = _p2->parse(str, idx);
		if(p2Success) { return true; }
		idx = i_idx; // rewind
		return false;
	}

	protected:
		Parser *_p1;
		Parser *_p2;
};

struct LiteralParser : public Parser {
	LiteralParser(string literal) : _literal(literal) { }
	bool parse(string &str, size_t &idx) {
		if(_literal.size() <= 0)
			return true;
		if(idx + _literal.size() - 1 >= str.size()) {
			cout << "literal parse of \"" << _literal << "\" failed at "
				<< idx << " not long enough" << endl;
			return false;
		}
		for(size_t i = 0; i < _literal.size(); ++i, ++idx) {
			if(str[idx] != _literal[i]) {
				cout << "literal parse of \"" << _literal << "\" failed at "
					<< idx << ": "
					<< str[idx] << " is not " << _literal[i] << endl;
				return false;
			}
		}
		return true;
	}

	protected:
		string _literal;
};


Parser *operator|(Parser &p1, Parser &p2);
Parser *operator+(Parser &p1, Parser &p2);
Parser *operator+(Parser *p1, Parser &p2);

Parser *operator|(Parser &p1, Parser &p2) { return new OrCombinator(p1, p2); }
Parser *operator+(Parser &p1, Parser &p2) { return new AndCombinator(p1, p2); }
Parser *operator+(Parser *p1, Parser &p2) { return new AndCombinator(*p1, p2); }



struct AtomParser : public Parser {
	AtomParser(string pseudoRegex);
	// how to yield token?... return Token*?
	bool parse(string &str, size_t &idx);

	private:
		string _pseudoRegex;
};

static string whiteSpace = " \t\r\n";
// take parser, return parser?
inline void ignoreWhitespace(string &str, size_t &idx);
inline void ignoreWhitespace(string &str, size_t &idx) {
	while(whiteSpace.find_first_of(str[idx]) != string::npos)
		idx++;
}

AtomParser::AtomParser(string pseudoRegex)
		: Parser(), _pseudoRegex(pseudoRegex) { }
bool AtomParser::parse(string &str, size_t &idx) {
	//ignoreWhitespace(str, idx);
	if(_pseudoRegex.find_first_of(str[idx]) == string::npos)
		return false;
	string atom;
	while(idx < str.size() && _pseudoRegex.find_first_of(str[idx]) != string::npos) {
		atom += str[idx++];
	}
	cout << "atom: " << atom << endl;
	return true;
}

void parser();
void parser() {
	auto quot = LiteralParser("\"");
	auto semicolon = LiteralParser(";");
	auto stringContent = AtomParser("Helo, wrd!");
	auto stringLiteral = quot + stringContent + quot;
	auto literal = stringLiteral;
	auto expression = literal;
	auto statement = expression + semicolon;

	statement->parse("\"Hello, world!\";");
}

int main(int argc, char **argv) {
	vector<string> args;
	for(int i = 1; i < argc; ++i)
		args.push_back(argv[i]);

	if(args.size() != 1) {
		cerr << "usage: " << argv[0] << " <source file>.st" << endl;
		return -1;
	}

	ifstream in(args.front());
	string line, source;
	size_t lineCount = 0;
	while(!in.eof() && in.good()) {
		getline(in, line);
		if(in.eof())
			break;
		source += line + "\n";
		lineCount++;
	}

	cout << source << endl;
	cout << lineCount << "," << source.size() << endl;

	cout << "parser: " << endl;
	parser();

	return 0;
}
