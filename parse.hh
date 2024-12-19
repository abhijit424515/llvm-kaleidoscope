#ifndef PARSE_HH
#define PARSE_HH

#include "common.hh"

extern std::string idt_str;
extern double num;

enum struct Token {
	eof = -1,
	def = -2,
	ext = -3,
	idt = -4,
	num = -5,
};

static int get_tok() {
	static int last = ' ';
	while (isspace(last))
		last = getchar();

	if (last == EOF)
		return static_cast<int>(Token::eof);

	if (isalpha(last)) {
		idt_str = last;
		while (isalnum(last = getchar()))
			idt_str += last;

		if (idt_str == "def")
			return static_cast<int>(Token::def);
		if (idt_str == "extern")
			return static_cast<int>(Token::ext);
		return static_cast<int>(Token::idt);
	}

	if (isdigit(last) || last == '.') {
		std::string num_str;
		do {
			num_str += last;
			last = getchar();
		} while (isdigit(last) || last == '.');

		num = strtod(num_str.c_str(), nullptr);
		return static_cast<int>(Token::num);
	}

	if (last == '#') {
		do
			last = getchar();
		while (last != EOF && last != '\n' && last != '\r');

		if (last != EOF)
			return get_tok();
	}

	int c = last;
	last = getchar();
	return c;
}

#endif