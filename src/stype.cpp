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
#include <algorithm>
using std::min;

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
	virtual bool parse(string &str, size_t &idx, bool printFailure = true) = 0;

	virtual void success(string &str, size_t sidx, size_t eidx) {
		if(name == "{null}")
			return;

		cout << name << ": '" << str.substr(sidx, eidx - sidx) << "'" << endl;
	}

	bool parse(string str, bool printFailure = true) {
		size_t idx = 0;
		bool s = parse(str, idx, printFailure);
		return s;
	}

	string name{"{null}"};
};

// print pretty error squigly
void printErrorDiagnostic(string &str, size_t &idx);

void printErrorDiagnostic(string &str, size_t &idx) {
	size_t cnt = min((size_t)3, idx), spaces = idx - cnt;
	cerr << str << endl
		<< string(spaces, ' ') << string(cnt, '~') << '^' << string(3, '~') << endl;
}

struct AndCombinator : public Parser {
	AndCombinator(Parser &p1, Parser &p2) : _p1(&p1), _p2(&p2) { }
	bool parse(string &str, size_t &idx, bool printFailure = true) {
		size_t i_idx = idx;
		bool p1Success = _p1->parse(str, idx, printFailure);
		if(!p1Success) {
			idx = i_idx;
			return false;
		}
		bool p2Success = _p2->parse(str, idx, printFailure);
		if(!p2Success) {
			idx = i_idx;
			return false;
		}
		success(str, i_idx, idx);
		return true;
	}

	protected:
		Parser *_p1;
		Parser *_p2;
};
struct OrCombinator : public Parser {
	OrCombinator(Parser &p1, Parser &p2) : _p1(&p1), _p2(&p2) { }
	bool parse(string &str, size_t &idx, bool printFailure = true) {
		size_t i_idx = idx;
		bool p1Success = _p1->parse(str, idx, false);
		if(p1Success) {
			success(str, i_idx, idx);
			return true;
		}
		idx = i_idx; // rewind
		bool p2Success = _p2->parse(str, idx, false);
		if(p2Success) {
			success(str, i_idx, idx);
			return true;
		}

		idx = i_idx; // rewind

		// we have failed, if we should have printed our failure rerun with
		// print enabled
		if(printFailure) {
			size_t a_idx = idx, b_idx = idx;
			_p1->parse(str, a_idx, printFailure);
			_p2->parse(str, b_idx, printFailure);
		}

		return false;
	}

	protected:
		Parser *_p1;
		Parser *_p2;
};

struct LiteralParser : public Parser {
	LiteralParser(string literal) : _literal(literal) { }
	bool parse(string &str, size_t &idx, bool printFailure = true) {
		size_t i_idx = idx; // TODO: does this not rollback?
		if(_literal.size() <= 0)
			return true;
		if(idx + _literal.size() - 1 >= str.size()) {
			if(printFailure)
				cout << "literal parse of \"" << _literal << "\" failed at "
					<< idx << " not long enough" << endl;
			return false;
		}
		for(size_t i = 0; i < _literal.size(); ++i, ++idx) {
			if(str[idx] != _literal[i]) {
				if(printFailure) {
					cout << "literal parse of \"" << _literal << "\" failed at "
						<< idx << ": "
						<< str[idx] << " is not " << _literal[i] << endl
						<< "        i: " << i << ", idx: " << idx << endl;
					printErrorDiagnostic(str, idx);
				}
				return false;
			}
		}
		success(str, i_idx, idx);
		return true;
	}

	protected:
		string _literal;
};

struct KleenePlusParser : public Parser {
	KleenePlusParser(Parser *parser) : _parser{parser} { }
	bool parse(string &str, size_t &idx, bool printFailure = true) {
		size_t i_idx = idx, scount = 0, s_idx = i_idx;
		while(_parser->parse(str, idx, false)) {
			s_idx = idx;
			scount++;
		}
		if(scount == 0) {
			idx = i_idx; // rewind
			if(printFailure) {
				size_t a_idx = idx;
				_parser->parse(str, a_idx, printFailure);
			}
			return false;
		}
		idx = s_idx;
		success(str, i_idx, idx);
		return true;
	}
	protected:
		Parser *_parser;
};


Parser *operator|(Parser &p1, Parser &p2);
Parser *operator|(Parser *p1, Parser &p2);
Parser *operator+(Parser &p1, Parser &p2);
Parser *operator+(Parser *p1, Parser &p2);
Parser *operator+(Parser &p1, Parser *p2);

Parser *operator|(Parser &p1, Parser &p2) { return new OrCombinator(p1, p2); }
Parser *operator|(Parser *p1, Parser &p2) { return new OrCombinator(*p1, p2); }
Parser *operator+(Parser &p1, Parser &p2) { return new AndCombinator(p1, p2); }
Parser *operator+(Parser *p1, Parser &p2) { return new AndCombinator(*p1, p2); }
Parser *operator+(Parser &p1, Parser *p2) { return new AndCombinator(p1, *p2); }



struct AtomParser : public Parser {
	AtomParser(string pseudoRegex);
	// how to yield token?... return Token*?
	bool parse(string &str, size_t &idx, bool printFailure = true);

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
bool AtomParser::parse(string &str, size_t &idx, bool printFailure) {
	size_t i_idx = idx;
	//ignoreWhitespace(str, idx);
	if(_pseudoRegex.find_first_of(str[idx]) == string::npos)
		return false;
	string atom;
	while(idx < str.size() && _pseudoRegex.find_first_of(str[idx]) != string::npos) {
		atom += str[idx++];
	}
	success(str, i_idx, idx);
	return true;
}

AtomParser CharacterRangeParser(char a, char b) {
	string s;
	for(char i = a; i <= b; i++)
		s += i;
	cerr << "CharacterRangeParser('" << a << "', '" << b << "'): " << s << endl;
	return AtomParser(s);
}

#define atom(x, y) auto x = y; x->name = #x ;

void parser();
void parser() {
	auto lowerAtom = CharacterRangeParser('a', 'z');
	auto upperAtom = CharacterRangeParser('A', 'Z');
	auto digitAtom = CharacterRangeParser('0', '9');
	auto spaceAtom = AtomParser(" \t\r\n");
	auto specialAtom = AtomParser(",!");
	auto graphAtom = lowerAtom | upperAtom | digitAtom | spaceAtom | specialAtom;
	auto escapedDoubleQuote = LiteralParser("\\\"");
	auto escapedBackslash = LiteralParser("\\\\");
	auto quot = LiteralParser("\"");
	auto semicolon = LiteralParser(";");
	auto stringContent = KleenePlusParser(graphAtom | escapedDoubleQuote | escapedBackslash);
	atom(stringLiteral, quot + stringContent + quot);
	auto literal = stringLiteral;
	auto expression = literal;
	auto statement = expression + semicolon;

	vector<string> tests{
		"\"Hello, \\\"worlD!\\\"\";",
		"\"Hello, \\\"worlD?\\\"\";",
	};
	for(auto &test : tests) {
		cout << "=== Test case: " << test << endl;
		statement->parse(test);
	}
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
