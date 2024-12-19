#ifndef AST_HH
#define AST_HH

#include "common.hh"

class Expr {
public:
	virtual ~Expr() = default;
};

class NumExpr : public Expr {
	double v;
public:
	NumExpr(double v) : v(v) {}
};

class VarExpr : public Expr {
	std::string name;
public:
	VarExpr(const std::string &name) : name(name) {}
};

class BinOpExpr : public Expr {
	char op;
	std::unique_ptr<Expr> lhs, rhs;

public:
	BinOpExpr(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
		: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

class CallExpr : public Expr {
	std::string callee;
	std::vector<std::unique_ptr<Expr>> args;

public:
	CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
		: callee(callee), args(std::move(args)) {}
};

class Prototype {
	std::string name;
	std::vector<std::string> args;

public:
	Prototype(const std::string &name, std::vector<std::string> args)
		: name(name), args(std::move(args)) {}

	const std::string &getName() const { return name; }
};

class Function {
	std::unique_ptr<Prototype> proto;
	std::unique_ptr<Expr> body;

public:
	Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
		: proto(std::move(proto)), body(std::move(body)) {}
};

#endif