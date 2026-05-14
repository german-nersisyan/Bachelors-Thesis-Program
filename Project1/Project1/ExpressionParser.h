#pragma once
#include <string>

class ExpressionParser {
public:
	double Evaluate(const std::string& expr, double x, double y);

private:
	double Parse(const std::string& expr);
	double ParseExpression(const std::string& expr, size_t& pos);
	double ParseTerm(const std::string& expr, size_t& pos);
	double ParsePower(const std::string& expr, size_t& pos);
	double ParseFactor(const std::string& expr, size_t& pos);
	void SkipSpaces(const std::string& expr, size_t& pos);
};