#pragma once
#include "PDEModel.h"

class PDESolverFrame;

class PDESolverController {
public:
	void SetView(PDESolverFrame* frame);
	void RunSolver();

	const PDEModel* GetModel() const { return &model; }

private:
	PDESolverFrame* view = nullptr;
	PDEModel model;
};
