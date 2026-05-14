#include "PDEModel.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cstdio>

void PDEModel::Initialize(const PDEParameters& params) {
	xMin = params.xMin; xMax = params.xMax;
	yMin = params.yMin; yMax = params.yMax;
	epsilon = params.epsilon;
	nx = params.internalNodes + 2;
	ny = params.internalNodes + 2;
	hx = (xMax - xMin) / (nx - 1);
	hy = (yMax - yMin) / (ny - 1);

	u.assign(ny, std::vector<double>(nx, 0.0));
	for (int j = 0; j < ny; ++j) {
		double y = yMin + j * hy;
		for (int i = 0; i < nx; ++i) {
			double x = xMin + i * hx;
			bool isBoundary = (i == 0 || i == nx - 1 || j == 0 || j == ny - 1);
			u[j][i] = isBoundary ? parser.Evaluate(params.boundaryFunc, x, y) : 0.0;
		}
	}
}

// Shared Gauss-Seidel (Liebmann) sweep updates u in-place, returns iteration count.
int PDEModel::RunLiebmann() {
	const bool uniform = fabs(hx - hy) < 1e-10;
	bool converged = false;
	int iter = 0;
	while (!converged && iter < 5000) {
		converged = true;
		iter++;
		for (int j = 1; j < ny - 1; ++j) {
			for (int i = 1; i < nx - 1; ++i) {
				double old = u[j][i];
				double u_new;
				if (uniform) {
					u_new = 0.25 * (u[j][i - 1] + u[j][i + 1] + u[j - 1][i] + u[j + 1][i]);
				}
				else {
					double wX = 1.0 / (hx * hx);
					double wY = 1.0 / (hy * hy);
					u_new = (wX * (u[j][i - 1] + u[j][i + 1]) +
						wY * (u[j - 1][i] + u[j + 1][i])) / (2.0 * (wX + wY));
				}
				u[j][i] = u_new;
				if (fabs(u_new - old) > epsilon) converged = false;
			}
		}
	}
	return iter;
}

static void PrintGrid(std::ostringstream& out, const std::vector<std::vector<double>>& u,
	int nx, int ny) {
	char buf[32];
	for (int j = ny - 1; j >= 0; --j) {
		for (int i = 0; i < nx; ++i) {
			snprintf(buf, sizeof(buf), "%10.4f ", u[j][i]);
			out << buf;
		}
		out << "\n";
	}
}

std::string PDEModel::SolveLiebmann() {
	std::ostringstream out;
	out << "LIEBMANN'S METHOD - Iteration Matrices\n";
	out << "======================================\n\n";

	const bool uniform = fabs(hx - hy) < 1e-10;
	if (uniform)
		out << "Uniform grid (hx = hy = h): using u = (u1+u2+u3+u4) / 4\n\n";
	else
		out << "Non-uniform grid (hx != hy): using weighted formula\n\n";

	// Run iteration-by-iteration so each matrix can be printed
	bool converged = false;
	int iter = 0;
	while (!converged && iter < 5000) {
		converged = true;
		iter++;
		for (int j = 1; j < ny - 1; ++j) {
			for (int i = 1; i < nx - 1; ++i) {
				double old = u[j][i];
				double u_new;
				if (uniform) {
					u_new = 0.25 * (u[j][i - 1] + u[j][i + 1] + u[j - 1][i] + u[j + 1][i]);
				}
				else {
					double wX = 1.0 / (hx * hx);
					double wY = 1.0 / (hy * hy);
					u_new = (wX * (u[j][i - 1] + u[j][i + 1]) +
						wY * (u[j - 1][i] + u[j + 1][i])) / (2.0 * (wX + wY));
				}
				u[j][i] = u_new;
				if (fabs(u_new - old) > epsilon) converged = false;
			}
		}
		out << "Iteration " << iter << ":\n---------------------\n";
		PrintGrid(out, u, nx, ny);
		out << "\n";
	}

	out << "\nConverged after " << iter << " iterations.\n";
	return out.str();
}

std::string PDEModel::SolveThomas() {
	std::ostringstream out;
	int nInt = nx - 2;
	double ax = 1.0 / (hx * hx);
	double by = 1.0 / (hy * hy);
	double d = -2 * ax - 2 * by;

	char buf[128];
	out << "THOMAS'S METHOD\n";
	out << "===============\n\n";

	// --- System matrix structure ---
	out << "Block tridiagonal system coefficients:\n";
	snprintf(buf, sizeof(buf), "  Main diagonal (d):        %10.4f\n", d);    out << buf;
	snprintf(buf, sizeof(buf), "  x off-diagonal (-ax):     %10.4f\n", -ax);  out << buf;
	snprintf(buf, sizeof(buf), "  y block off-diagonal (-by):%10.4f\n\n", -by); out << buf;

	out << "Block Tridiagonal System Matrix Structure:\n";
	int displaySize = std::min(nInt * nInt, 9);
	for (int r = 0; r < displaySize; r++) {
		for (int c = 0; c < displaySize; c++) {
			double val = 0;
			if (r == c) val = d;
			else if (c == r + 1 && (c % nInt != 0)) val = -ax;
			else if (c == r - 1 && (r % nInt != 0)) val = -ax;
			else if (c == r + nInt) val = -by;
			else if (c == r - nInt) val = -by;
			if (val == 0) { out << "    0   "; }
			else { snprintf(buf, sizeof(buf), "%7.2f ", val); out << buf; }
		}
		out << "\n";
	}
	if (nInt * nInt > 9) {
		snprintf(buf, sizeof(buf), "\n(Showing %dx%d of full %dx%d system)\n",
			displaySize, displaySize, nInt*nInt, nInt*nInt);
		out << buf;
	}
	out << "\n";

	// --- Step 1: solve linear PDE (delta u = 0) with Liebmann initial values u_0 ---
	const bool uniform = fabs(hx - hy) < 1e-10;
	out << "Step 1: Solve linear PDE (delta u = 0) with Liebmann's method";
	if (uniform)
		out << " [uniform grid: u = (u1+u2+u3+u4)/4]";
	out << "\n";
	out << std::string(60, '-') << "\n";

	int liebmann_iter = RunLiebmann();

	snprintf(buf, sizeof(buf), "Liebmann converged after %d iterations. Initial values u_0:\n", liebmann_iter);
	out << buf;
	out << "---------------------\n";
	PrintGrid(out, u, nx, ny);
	out << "\n";

	// --- Step 2: solve non-linear PDE (delta u = e^u) by successive approximations ---
	// Each iteration: build RHS F(u^(k)) = e^(u^(k)), solve A*u^(k+1) = F using Thomas algorithm
	out << "Step 2: Solve non-linear PDE (delta u = e^u) by successive approximations\n";
	out << "         Each iteration: solve A*u^(k+1) = e^(u^(k)) via Thomas (block tridiagonal)\n";
	out << std::string(70, '-') << "\n\n";

	bool converged = false;
	int iter = 0;
	while (!converged && iter < 2000) {
		iter++;

		// Build RHS: F[j][k] = -e^(u^(k-1)) + boundary contributions
		// (negated because we solve -A * u = -F, i.e. the positive-definite form)
		std::vector<std::vector<double>> F(nInt, std::vector<double>(nInt, 0.0));
		for (int j = 1; j <= nInt; j++) {
			for (int k = 1; k <= nInt; k++) {
				F[j - 1][k - 1] = -exp(std::min(u[j][k], 20.0));
				if (k == 1)    F[j - 1][k - 1] += ax * u[j][0];
				if (k == nInt) F[j - 1][k - 1] += ax * u[j][nInt + 1];
				if (j == 1)    F[j - 1][k - 1] += by * u[0][k];
				if (j == nInt) F[j - 1][k - 1] += by * u[nInt + 1][k];
			}
		}

		// Solve A*u^(k+1) = F using Thomas (block tridiagonal) algorithm
		auto sol = SolveBlockTridiagonal(F, ax, by, nInt);

		// Successive approximation update (pure, no relaxation)
		double maxdiff = 0;
		for (int j = 1; j <= nInt; j++) {
			for (int k = 1; k <= nInt; k++) {
				double diff = fabs(sol[j - 1][k - 1] - u[j][k]);
				maxdiff = std::max(maxdiff, diff);
				u[j][k] = sol[j - 1][k - 1];
			}
		}
		converged = (maxdiff < epsilon);

		snprintf(buf, sizeof(buf), "Iteration %d (max change = %.6f):\n", iter, maxdiff);
		out << buf;
		out << "---------------------\n";
		PrintGrid(out, u, nx, ny);
		out << "\n";
	}

	snprintf(buf, sizeof(buf), "Converged after %d successive approximation iterations.\n", iter);
	out << buf;
	return out.str();
}

std::vector<double> PDEModel::SolveTridiagonal(
	std::vector<double> a, std::vector<double> b,
	std::vector<double> c, std::vector<double> d) {

	int n = (int)b.size();
	for (int i = 1; i < n; i++) {
		double m = a[i] / b[i - 1];
		b[i] -= m * c[i - 1];
		d[i] -= m * d[i - 1];
	}
	std::vector<double> x(n);
	x[n - 1] = d[n - 1] / b[n - 1];
	for (int i = n - 2; i >= 0; i--)
		x[i] = (d[i] - c[i] * x[i + 1]) / b[i];
	return x;
}

std::vector<std::vector<double>> PDEModel::SolveBlockTridiagonal(
	std::vector<std::vector<double>>& F, double ax, double by, int n) {

	double diag = 2 * ax + 2 * by;
	std::vector<double> lower(n, -ax), main_d(n, diag), upper(n, -ax);

	std::vector<std::vector<double>> beta(n, std::vector<double>(n));
	std::vector<std::vector<double>> U(n, std::vector<double>(n));

	beta[0] = SolveTridiagonal(lower, main_d, upper, F[0]);
	for (int j = 1; j < n; j++) {
		std::vector<double> rhs(n);
		for (int i = 0; i < n; i++) rhs[i] = F[j][i] + by * beta[j - 1][i];
		beta[j] = SolveTridiagonal(lower, main_d, upper, rhs);
	}

	U[n - 1] = beta[n - 1];
	for (int j = n - 2; j >= 0; j--)
		for (int i = 0; i < n; i++)
			U[j][i] = beta[j][i] + by * U[j + 1][i];

	return U;
}