#include "parse.hh"
#include "ast.hh"

std::string idt_str;
double num;
static int curr_tok;

//===----------------------------------------------------------------------===//
// Error handling
//===----------------------------------------------------------------------===//

std::unique_ptr<Expr> log_error(const char *str) {
	std::cerr << "[error] " << str << std::endl;
	return nullptr;
}

std::unique_ptr<Prototype> log_error_p(const char *str) {
	log_error(str);
	return nullptr;
}

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

static int get_next_token() {
	return curr_tok = get_tok();
}

static const std::unordered_map<char, int> binop_precedence = {
	{ '<', 10 },
	{ '+', 20 },
	{ '-', 20 },
	{ '*', 40 },
};

static int get_tok_precedence() {
	if (!isascii(curr_tok))
		return -1;
	if (!binop_precedence.count(curr_tok))
		return -1;
	
	int tok_prec = binop_precedence.at((char)curr_tok);
	return tok_prec;
}

static std::unique_ptr<Expr> parse_expression();

// numberexpr ::= number
static std::unique_ptr<Expr> parse_number_expr() {
	auto result = std::make_unique<NumExpr>(num);
	get_next_token();
	return std::move(result);
}

// parenexpr ::= '(' expression ')'
static std::unique_ptr<Expr> parse_paren_expr() {
	get_next_token();
	auto v = parse_expression();
	if (!v)
		return nullptr;
	
	if (curr_tok != ')')
		return log_error("expected ')'");
	get_next_token();
	return v;
}

// identifierexpr
//   ::= identifier
//   ::= identifier '(' expression* ')'
static std::unique_ptr<Expr> parse_identifier_expr() {
	std::string id = idt_str;
	get_next_token();
	
	if (curr_tok != '(')
		return std::make_unique<VarExpr>(id);
	
	get_next_token();
	std::vector<std::unique_ptr<Expr>> args;
	if (curr_tok != ')') {
		while (1) {
			if (auto arg = parse_expression())
				args.push_back(std::move(arg));
			else
				return nullptr;
			
			if (curr_tok == ')')
				break;
			
			if (curr_tok != ',')
				return log_error("Expected ')' or ',' in argument list");
			get_next_token();
		}
	}
	
	get_next_token();
	return std::make_unique<CallExpr>(id, std::move(args));
}

// primary
//   ::= identifierexpr
//   ::= numberexpr
//   ::= parenexpr
static std::unique_ptr<Expr> parse_primary() {
	switch (curr_tok) {
	case static_cast<int>(Token::idt):
		return parse_identifier_expr();
	case static_cast<int>(Token::num):
		return parse_number_expr();
	case '(':
		return parse_paren_expr();
	default:
		return log_error("unknown token when expecting an expression");
	}
}

// binoprhs
//   ::= ('+' primary)*
static std::unique_ptr<Expr> parse_binop_rhs(int expr_prec, std::unique_ptr<Expr> lhs) {
	while (1) {
		int tok_prec = get_tok_precedence();
		if (tok_prec < expr_prec)
			return lhs;
		
		int binop = curr_tok;
		get_next_token();
		
		auto rhs = parse_primary();
		if (!rhs)
			return nullptr;
		
		int next_prec = get_tok_precedence();
		if (tok_prec < next_prec) {
			rhs = parse_binop_rhs(tok_prec + 1, std::move(rhs));
			if (!rhs)
				return nullptr;
		}
		
		lhs = std::make_unique<BinOpExpr>(binop, std::move(lhs), std::move(rhs));
	}
}

// expression
//   ::= primary binoprhs
static std::unique_ptr<Expr> parse_expression() {
	auto lhs = parse_primary();
	if (!lhs)
		return nullptr;
	
	return parse_binop_rhs(0, std::move(lhs));
}

// prototype
//   ::= id '(' id* ')'
static std::unique_ptr<Prototype> parse_prototype() {
	if (curr_tok != static_cast<int>(Token::idt))
		return log_error_p("Expected function name in prototype");
	
	std::string fn_name = idt_str;
	get_next_token();
	
	if (curr_tok != '(')
		return log_error_p("Expected '(' in prototype");
	
	std::vector<std::string> arg_names;
	while (get_next_token() == static_cast<int>(Token::idt))
		arg_names.push_back(idt_str);
	
	if (curr_tok != ')')
		return log_error_p("Expected ')' in prototype");
	
	get_next_token();
	return std::make_unique<Prototype>(fn_name, std::move(arg_names));
}

// definition ::= 'def' prototype expression
static std::unique_ptr<Function> parse_definition() {
	get_next_token();
	auto proto = parse_prototype();
	if (!proto)
		return nullptr;
	
	if (auto expr = parse_expression())
		return std::make_unique<Function>(std::move(proto), std::move(expr));
	return nullptr;
}

// toplevelexpr ::= expression
static std::unique_ptr<Function> parse_top_level_expr() {
	if (auto expr = parse_expression()) {
		auto proto = std::make_unique<Prototype>("", std::vector<std::string>());
		return std::make_unique<Function>(std::move(proto), std::move(expr));
	}
	return nullptr;
}

// external ::= 'extern' prototype
static std::unique_ptr<Prototype> parse_extern() {
	get_next_token();
	return parse_prototype();
}

//===----------------------------------------------------------------------===//
// Top-Level parsing
//===----------------------------------------------------------------------===//

// definition ::= 'def' prototype expression
void handle_definition() {
	if (parse_definition())
		std::cerr << "Parsed a function definition.\n";
	else
		get_next_token();
}

// external ::= 'extern' prototype
void handle_extern() {
	if (parse_extern())
		std::cerr << "Parsed an extern.\n";
	else
		get_next_token();
}

// expression ::= primary binoprhs
void handle_top_level_expr() {
	if (parse_top_level_expr())
		std::cerr << "Parsed a top-level expression.\n";
	else
		get_next_token();
}