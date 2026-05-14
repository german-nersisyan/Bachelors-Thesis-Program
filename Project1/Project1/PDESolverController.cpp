#include "PDESolverController.h"
#include "PDESolverFrame.h"
#include <wx/wx.h>

void PDESolverController::SetView(PDESolverFrame* frame) {
	view = frame;
}

void PDESolverController::RunSolver() {
	PDEParameters params;
	params.xMin = view->GetXMin();
	params.xMax = view->GetXMax();
	params.yMin = view->GetYMin();
	params.yMax = view->GetYMax();
	params.internalNodes = view->GetGridSize();
	params.boundaryFunc = std::string(view->GetBoundaryFunc().mb_str());
	params.epsilon = view->GetEpsilon();
	if (params.epsilon <= 0) params.epsilon = 0.01;

	try {
		model.Initialize(params);
	}
	catch (...) {
		wxMessageBox("Error parsing boundary function. Using default: 25-x^2", "Parse Error");
		params.boundaryFunc = "25-x^2";
		model.Initialize(params);
	}

	std::string output;
	if (view->GetMethod() == 0)
		output = model.SolveLiebmann();
	else
		output = model.SolveThomas();

	view->SetMatrixOutput(wxString(output));
	view->RefreshCanvas();
}