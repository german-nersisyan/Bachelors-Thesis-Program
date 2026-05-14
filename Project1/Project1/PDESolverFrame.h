#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>

class PDESolverController;

class PDESolverFrame : public wxFrame {
public:
	explicit PDESolverFrame(PDESolverController* ctrl);

	// Input accessors for the controller
	double   GetXMin()        const;
	double   GetXMax()        const;
	double   GetYMin()        const;
	double   GetYMax()        const;
	int      GetGridSize()    const;
	wxString GetBoundaryFunc() const;
	double   GetEpsilon()     const;
	int      GetMethod()      const;

	// Output setters called by the controller
	void SetMatrixOutput(const wxString& text);
	void RefreshCanvas();

private:
	PDESolverController* controller;

	wxTextCtrl  *xMinCtrl, *xMaxCtrl, *yMinCtrl, *yMaxCtrl;
	wxTextCtrl  *boundaryFuncCtrl, *epsilonCtrl, *matrixPanel;
	wxSpinCtrl  *gridSizeCtrl;
	wxRadioBox  *methodBox;
	wxNotebook  *notebook;
	wxPanel     *canvas;

	double zoomFactor;
	int hoverI, hoverJ;

	void OnSolve(wxCommandEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);
};