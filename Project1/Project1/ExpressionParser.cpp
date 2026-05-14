#include "ExpressionParser.h"
#include <cmath>
#include <cctype>
#include <stdexcept>

double ExpressionParser::Evaluate(const std::string& expr, double x, double y) {
	std::string formatted = expr;

	auto replaceVar = [](std::string& s, char var, double val) {
		size_t pos = 0;
		while ((pos = s.find(var, pos)) != std::string::npos) {
			bool prevOk = (pos == 0 || !isalnum(s[pos - 1]));
			bool nextOk = (pos + 1 >= s.length() || !isalpha(s[pos + 1]));
			if (prevOk && nextOk) {
				std::string rep = "(" + std::to_string(val) + ")";
				s.replace(pos, 1, rep);
				pos += rep.length();
			}
			else {
				pos++;
			}
		}
	};

	replaceVar(formatted, 'x', x);
	replaceVar(formatted, 'y', y);
	return Parse(formatted);
}

double ExpressionParser::Parse(const std::string& expr) {
	size_t pos = 0;
	return ParseExpression(expr, pos);
}

double ExpressionParser::ParseExpression(const std::string& expr, size_t& pos) {
	double result = ParseTerm(expr, pos);
	while (pos < expr.length()) {
		SkipSpaces(expr, pos);
		if (pos >= expr.length()) break;
		if (expr[pos] == '+') { pos++; result += ParseTerm(expr, pos); }
		else if (expr[pos] == '-') { pos++; result -= ParseTerm(expr, pos); }
		else break;
	}
	return result;
}

double ExpressionParser::ParseTerm(const std::string& expr, size_t& pos) {
	double result = ParsePower(expr, pos);
	while (pos < expr.length()) {
		SkipSpaces(expr, pos);
		if (pos >= expr.length()) break;
		if (expr[pos] == '*') {
			pos++;
			result *= ParsePower(expr, pos);
		}
		else if (expr[pos] == '/') {
			pos++;
			double divisor = ParsePower(expr, pos);
			if (fabs(divisor) < 1e-10) throw std::runtime_error("Division by zero");
			result /= divisor;
		}
		else break;
	}
	return result;
}

double ExpressionParser::ParsePower(const std::string& expr, size_t& pos) {
	double result = ParseFactor(expr, pos);
	SkipSpaces(expr, pos);
	if (pos < expr.length() && expr[pos] == '^') {
		pos++;
		result = pow(result, ParsePower(expr, pos));
	}
	return result;
}

double ExpressionParser::ParseFactor(const std::string& expr, size_t& pos) {
	SkipSpaces(expr, pos);

	if (pos < expr.length() && expr[pos] == '-') { pos++; return -ParseFactor(expr, pos); }
	if (pos < expr.length() && expr[pos] == '+') { pos++; return ParseFactor(expr, pos); }

	if (pos < expr.length() && expr[pos] == '(') {
		pos++;
		double result = ParseExpression(expr, pos);
		SkipSpaces(expr, pos);
		if (pos < expr.length() && expr[pos] == ')') pos++;
		return result;
	}

	if (pos < expr.length() && isalpha(expr[pos])) {
		std::string func;
		while (pos < expr.length() && isalpha(expr[pos])) func += expr[pos++];
		SkipSpaces(expr, pos);
		if (pos < expr.length() && expr[pos] == '(') {
			pos++;
			double arg = ParseExpression(expr, pos);
			SkipSpaces(expr, pos);
			if (pos < expr.length() && expr[pos] == ')') pos++;
			if (func == "sin")  return sin(arg);
			if (func == "cos")  return cos(arg);
			if (func == "exp")  return exp(arg);
			if (func == "sqrt") return sqrt(arg);
			if (func == "abs" || func == "fabs") return fabs(arg);
			if (func == "tan")  return tan(arg);
			if (func == "ln" || func == "log") return log(arg);
			throw std::runtime_error("Unknown function: " + func);
		}
	}

	if (pos < expr.length() && (isdigit(expr[pos]) || expr[pos] == '.')) {
		size_t endPos;
		double result = stod(expr.substr(pos), &endPos);
		pos += endPos;
		return result;
	}

	throw std::runtime_error("Unexpected character in expression");
}

void ExpressionParser::SkipSpaces(const std::string& expr, size_t& pos) {
	while (pos < expr.
		length() && isspace(expr[pos])) pos++;
}