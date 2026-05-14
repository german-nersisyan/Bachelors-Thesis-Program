#pragma once
#include <vector>
#include <string>
#include "ExpressionParser.h"

struct PDEParameters {
	double xMin, xMax, yMin, yMax;
	int internalNodes;
	std::string boundaryFunc;
	double epsilon;
};

class PDEModel {
public:
	void Initialize(const PDEParameters& params);
	std::string SolveLiebmann();
	std::string SolveThomas();

	const std::vector<std::vector<double>>& GetSolution() const { return u; }
	double GetXMin() const { return xMin; }
	double GetXMax() const { return xMax; }
	double GetYMin() const { return yMin; }
	double GetYMax() const { return yMax; }
	double GetHx()   const { return hx; }
	double GetHy()   const { return hy; }
	int    GetNx()   const { return nx; }
	int    GetNy()   const { return ny; }

private:
	double xMin = 0, xMax = 0, yMin = 0, yMax = 0, hx = 0, hy = 0, epsilon = 0.01;
	int nx = 0, ny = 0;
	std::vector<std::vector<double>> u;
	ExpressionParser parser;

	int RunLiebmann();  // returns iteration count; updates u in-place

	std::vector<double> SolveTridiagonal(
		std::vector<double> a, std::vector<double> b,
		std::vector<double> c, std::vector<double> d);

	std::vector<std::vector<double>> SolveBlockTridiagonal(
		std::vector<std::vector<double>>& F, double ax, double by, int n);
};