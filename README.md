# PDE Solver — Bachelor Diploma Thesis

A desktop application for solving partial differential equations (PDEs) numerically on a 2D rectangular grid using two classical iterative methods: Liebmann's Method and Thomas's Method.

Built with C++ and wxWidgets.

---

# Problem Statement

The application solves two types of boundary value problems on a rectangular domain:

[xMin, xMax] × [yMin, yMax]

| Problem        | Equation                  | Method            |
| -------------- | ------------------------- | ----------------- |
| Linear PDE     | Δu = 0 (Laplace equation) | Liebmann's Method |
| Non-linear PDE | Δu = eᵘ                   | Thomas's Method   |

Boundary conditions are defined by a user-supplied expression evaluated along the boundary.

---

# Numerical Methods

## Liebmann's Method (Gauss-Seidel Iteration)

Solves the linear Laplace equation Δu = 0 by iteratively updating each interior node:

### Uniform grid (hx = hy = h)

u[i,j] = (u[i-1,j] + u[i+1,j] + u[i,j-1] + u[i,j+1]) / 4

### Non-uniform grid (hx ≠ hy)

Weighted average using:

* wX = 1/hx²
* wY = 1/hy²

Iterations continue until the maximum change between iterations falls below epsilon ε.

---

## Thomas's Method (Method of Successive Approximations)

Solves the non-linear equation:

Δu = eᵘ

in two steps:

### Step 1 — Initial values

Solve the linear problem Δu = 0 using Liebmann's method to obtain u₀.

### Step 2 — Successive approximations

At each iteration k, fix the non-linear term using the previous iterate and solve the resulting linear block tridiagonal system:

A · u^(k+1) = F(u^(k))

where:

F[i,j] = e^(u^(k)[i,j])

Each linear system is solved exactly using the Thomas (block tridiagonal) algorithm, which performs:

* Forward sweep (LU factorization row by row)
* Backward substitution

Iterations stop when:

max|u^(k+1) - u^(k)| < ε

---

# Features

* Interactive GUI built with wxWidgets
* Configurable domain:

  * x/y range
  * number of internal nodes
  * epsilon
* Custom boundary function parser supporting:

  * +, -, *, /, ^
  * sin, cos, tan
  * exp, sqrt, abs, ln
* Visual Grid tab:

  * coordinate system with color-coded grid points
  * red = boundary
  * green = interior
  * hover tooltips showing node names and values
  * zoom via Ctrl+Scroll or Ctrl++/-
* Matrices tab:

  * full iteration-by-iteration output of solution matrices for both methods

---

# Architecture

The project follows the MVC (Model-View-Controller) pattern:

* ExpressionParser — pure math: tokenizes and evaluates boundary function strings
* PDEModel — grid data, Liebmann solver, Thomas solver, tridiagonal helpers
* PDESolverController — reads inputs from view, drives model, pushes results back
* PDESolverFrame — wxFrame UI: controls sidebar, canvas, matrix output panel
* App / main.cpp — application entry point

---

# Build Requirements

* C++17 or later
* wxWidgets 3.x

## Linux

(wxWidgets installed)

```bash
g++ -std=c++17 $(wx-config --cxxflags) \
main.cpp App.cpp \
ExpressionParser.cpp \
PDEModel.cpp \
PDESolverController.cpp \
PDESolverFrame.cpp \
$(wx-config --libs) -o pde_solver
```

## Windows (Visual Studio)

Add all `.cpp` files to the project and link against the wxWidgets libraries.

---

# Usage

1. Build and launch the application
2. Fill in the domain settings:

### Domain Parameters

* **X Range / Y Range** — rectangular domain bounds
* **Internal Nodes** — number of interior grid points per axis (n), giving an (n+2) × (n+2) total grid
* **Boundary Function** — expression evaluated on all boundary nodes (e.g. `25 - x^2`)
* **Epsilon** — convergence tolerance (e.g. `0.01`)

3. Select a method:

* **Liebmann's Method** — for the linear Laplace equation
* **Thomas's Method** — for the non-linear equation Δu = eᵘ

4. Click **Run Solver**

5. Inspect results:

### Visual Grid tab

* See the solution plotted on the coordinate plane
* Hover over nodes for values

### Matrices tab

* Read the full iteration log

---

# File Structure

```text
├── main.cpp                     # Entry point (wxIMPLEMENT_APP)
├── App.h / App.cpp              # wxApp subclass, wires controller and view
├── ExpressionParser.h/.cpp      # Recursive-descent math expression parser
├── PDEModel.h/.cpp              # Numerical solvers and grid state (Model)
├── PDESolverController.h/.cpp   # Input → model → output bridge (Controller)
└── PDESolverFrame.h/.cpp        # wxWidgets GUI (View)
```
