# PDE Solver â€" Bachelor Diploma Thesis

A desktop application for solving partial differential equations (PDEs) numerically on a 2D rectangular grid using two classical iterative methods: Liebmann's Method and Thomas's Method.

Built with C++ and wxWidgets.

---

## Problem Statement

The application solves two types of boundary value problems on a rectangular domain [xMin, xMax] Ã— [yMin, yMax]:

| Problem | Equation | Method |
|---|---|---|
| Linear PDE | Î"u = 0 (Laplace equation) | Liebmann's Method |
| Non-linear PDE | Î"u = eáµ˜ | Thomas's Method (successive approximations) |

Boundary conditions are defined by a user-supplied expression evaluated along the boundary.

---

## Numerical Methods

### Liebmann's Method (Gauss-Seidel Iteration)
Solves the linear Laplace equation Î"u = 0 by iteratively updating each interior node:

- Uniform grid (hx = hy = h):
   u[i,j] = (u[i-1,j] + u[i+1,j] + u[i,j-1] + u[i,j+1]) / 4
  - Non-uniform grid (hx â‰  hy): weighted average using wX = 1/hxÂ², wY = 1/hyÂ²

Iterations continue until the maximum change between iterations falls below epsilon Îµ.

### Thomas's Method (Method of Successive Approximations)
Solves the non-linear equation Î"u = eáµ˜ in two steps:

1. Step 1 â€" Initial values: solve the linear problem Î"u = 0 using Liebmann's method to obtain uâ‚€
2. Step 2 â€" Successive approximations: at each iteration k, fix the non-linear term using the previous iterate and solve the resulting linear block tridiagonal system:

     A Â· u^(k+1) = F(u^(k)),   where F[i,j] = e^(u^(k)[i,j])
   
   Each linear system is solved exactly using the Thomas (block tridiagonal) algorithm, which performs:
   - Forward sweep (LU factorization row by row)
   - Backward substitution

   Iterations stop when max|u^(k+1) - u^(k)| < Îµ.

---

## Features

- Interactive GUI built with wxWidgets
- Configurable domain: x/y range, number of internal nodes, epsilon
- Custom boundary function parser supporting: +, -, *, /, ^, sin, cos, tan, exp, sqrt, abs, ln
- Visual Grid tab: coordinate system with color-coded grid points (red = boundary, green = interior), hover tooltips showing node names and values, zoom via Ctrl+Scroll or Ctrl+`+`/`-`
- Matrices tab: full iteration-by-iteration output of solution matrices for both methods

---

## Architecture

The project follows the MVC (Model-View-Controller) pattern:
ExpressionParser   â€" pure math: tokenizes and evaluates boundary function strings
PDEModel           â€" grid data, Liebmann solver, Thomas solver, tridiagonal helpers
PDESolverController â€" reads inputs from view, drives model, pushes results back
PDESolverFrame     â€" wxFrame UI: controls sidebar, canvas, matrix output panel
App / main.cpp     â€" application entry point

---

## Build Requirements

- C++17 or later
- [wxWidgets](https://www.wxwidgets.org/) 3.x

### Linux (wxWidgets installed)g++ -std=c++17 $(wx-config --cxxflags) \
    main.cpp App.cpp \
    ExpressionParser.cpp \
    PDEModel.cpp \
    PDESolverController.cpp \
    PDESolverFrame.cpp \
    $(wx-config --libs) -o pde_solver

### Windows (Visual Studio)
Add all .cpp files to the project and link against the wxWidgets libraries.

---

## Usage

1. Build and launch the application
2. Fill in the domain settings:
   - X Range / Y Range â€" rectangular domain bounds
   - Internal Nodes â€" number of interior grid points per axis (n), giving an (n+2)Ã—(n+2) grid total
   - Boundary Function â€" expression evaluated on all boundary nodes (e.g. `25 - x^2`)
   - Epsilon â€" convergence tolerance (e.g. `0.01`)
3. Select a method:
   - Liebmann's Method â€" for the linear Laplace equation
   - Thomas's Method â€" for the non-linear equation Î"u = eáµ˜
4. Click Run Solver
5. Inspect results:
   - Visual Grid tab â€" see the solution plotted on the coordinate plane; hover over nodes for values
   - Matrices tab â€" read the full iteration log

---

## File Structure
â"œâ"€â"€ main.cpp                  # Entry point (wxIMPLEMENT_APP)
â"œâ"€â"€ App.h / App.cpp           # wxApp subclass, wires controller and view
â"œâ"€â"€ ExpressionParser.h/.cpp   # Recursive-descent math expression parser
â"œâ"€â"€ PDEModel.h/.cpp           # Numerical solvers and grid state (Model)
â"œâ"€â"€ PDESolverController.h/.cpp# Input â†’ model â†’ output bridge (Controller)
â""â"€â"€ PDESolverFrame.h/.cpp     # wxWidgets GUI (View)