#include <iostream>
#include <string>
#include <cctype>
#include <stack>

// Base class for all AST nodes
class Expr {
public:
	virtual ~Expr() = default;
};

// Number: AST leaf node
class Num : public Expr {
private:
	int val;
public:
	Num(int val) : val(val) {}
	int getVal() const { return val; }
};

// Binary operation: internal AST node
class Op : public Expr {
private:
	char op;
	Expr* left;
	Expr* right;
public:
	Op(char op, Expr* left, Expr* right) : op(op), left(left), right(right) {}
	~Op() { delete left; delete right; }
	char getOp() const { return op; }
	Expr* getLeft() const { return left; }
	Expr* getRight() const { return right; }
};

Expr* ParseToAST(const std::string& expr) {
	std::stack<Expr*> values;
	std::stack<char> ops;
	auto priority = [](char op) -> int {
		return (op == '*' || op == '/') ? 2 :
			(op == '+' || op == '-') ? 1 : 0;
		};

	int i = 0;
	while (i < expr.size()) {
		char c = expr[i];
		if (isspace(c)) {
			++i;
			continue;
		}
		if (isdigit(c)) {
			std::string numStr;
			while (i < expr.size() && isdigit(expr[i])) {
				numStr += expr[i];
				++i;
			}
			std::cout << "NumberLexeme:" << numStr << std::endl;
			values.push(new Num(std::stoi(numStr)));
			continue;
		}
		else if (c == '-' && (i == 0 || expr[i - 1] == '(' ||
			(i > 0 && (expr[i - 1] == '+' || expr[i - 1] == '-' ||
				expr[i - 1] == '*' || expr[i - 1] == '/')))) {
			std::string numStr = "-";
			++i;
			while (i < expr.size() && isdigit(expr[i])) {
				numStr += expr[i];
				++i;
			}
			std::cout << "NumberLexeme:" << numStr << std::endl;
			values.push(new Num(std::stoi(numStr)));
			continue;
		}
		else if (c == '+' || c == '-' || c == '*' || c == '/') {
			std::cout << "Operator: " << c << std::endl;
			while (!ops.empty() && priority(ops.top()) >= priority(c)) {
				char op = ops.top();
				ops.pop();
				Expr* right = values.top();
				values.pop();
				Expr* left = values.top();
				values.pop();
				values.push(new Op(op, left, right));
			}
			ops.push(c);
			++i;
		}
		else if (c == '(') {
			std::cout << "Delimiter '('" << std::endl;
			ops.push(c);
			++i;
		}
		else if (c == ')') {
			std::cout << "Delimiter ')'" << std::endl;
			while (!ops.empty() && ops.top() != '(') {
				char op = ops.top();
				ops.pop();
				Expr* right = values.top();
				values.pop();
				Expr* left = values.top();
				values.pop();
				values.push(new Op(op, left, right));
			}
			ops.pop();
			++i;
		}
	}
	std::cout << "====================" << std::endl;
	while (!ops.empty()) {
		char op = ops.top();
		ops.pop();
		Expr* right = values.top();
		values.pop();
		Expr* left = values.top();
		values.pop();
		values.push(new Op(op, left, right));
	}
	return values.empty() ? nullptr : values.top();
}
void print(Expr* expr, const std::string& space = "") {
	auto op = dynamic_cast<Op*>(expr);
	if (op) {
		std::cout << space << op->getOp() << std::endl;
		print(op->getLeft(), space + " ");
		print(op->getRight(), space + " ");
	}
	else {
		auto num = dynamic_cast<Num*>(expr);
		if (num) {
			std::cout << space << num->getVal() << std::endl;
		}
	}
}

void Gen(Expr* expr) {
	auto op = dynamic_cast<Op*>(expr);
	if (op) {
		Gen(op->getLeft());
		std::cout << "push %rax" << std::endl;
		Gen(op->getRight());
		std::cout << "mov %rax, %rbx" << std::endl;
		std::cout << "pop %rax" << std::endl;
		switch (op->getOp()) {
		case '+':
			std::cout << "add %rbx, %rax" << std::endl;
			break;
		case '-':
			std::cout << "sub %rbx, %rax" << std::endl;
			break;
		case '*':
			std::cout << "imul %rbx, %rax" << std::endl;
			break;
		case '/':
			std::cout << "idiv %rbx, %rax" << std::endl;;
			break;
		default:
			std::cout << "unknown op" << std::endl;
		}
	}
	else {
		auto num = dynamic_cast<Num*>(expr);
		if (num) {
			std::cout << "mov $" << num->getVal() << ", %rax" << std::endl;
		}
	}
}


int main()
{
	std::string expression = "((100*(-100))+(200*100))*2";
	auto a = ParseToAST(expression);
	print(a);
	std::cout << "====================" << std::endl;
	std::cout << ".text" << '\n' << "Num:" << std::endl;
	Gen(a);
	std::cout << "toStr" << '\n' << "print" << '\n' << "jmp End" << std::endl;
	std::cout << ".globl _start" << '\n' << "_start:" << '\n' << "call Num" << std::endl;
	std::cout << "End:" << '\n' << "mov $60,%rax" << '\n' << "xor %rdi,%rdi" << '\n' << "syscall" << std::endl;
	return 0;
}
