#include "PDESolverFrame.h"
#include "PDESolverController.h"
#include "PDEModel.h"
#include <cmath>

PDESolverFrame::PDESolverFrame(PDESolverController* ctrl)
	: wxFrame(nullptr, wxID_ANY, "PDE Solver", wxDefaultPosition, wxSize(1300, 900))
	, controller(ctrl)
	, zoomFactor(45.0)
	, hoverI(-1), hoverJ(-1)
{
	wxPanel* mainPanel = new wxPanel(this);
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

	wxPanel* sidebar = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(300, -1));
	sidebar->SetBackgroundColour(wxColour(240, 240, 240));
	wxStaticBoxSizer* ctrlSizer = new wxStaticBoxSizer(wxVERTICAL, sidebar, "Settings");

	xMinCtrl = new wxTextCtrl(sidebar, wxID_ANY, "");
	xMaxCtrl = new wxTextCtrl(sidebar, wxID_ANY, "");
	yMinCtrl = new wxTextCtrl(sidebar, wxID_ANY, "");
	yMaxCtrl = new wxTextCtrl(sidebar, wxID_ANY, "");
	gridSizeCtrl = new wxSpinCtrl(sidebar, wxID_ANY, "0", wxDefaultPosition,
		wxDefaultSize, wxSP_ARROW_KEYS, 0, 20, 0);

	ctrlSizer->Add(new wxStaticText(sidebar, wxID_ANY, "X Range:"), 0, wxTOP, 5);
	ctrlSizer->Add(xMinCtrl, 0, wxEXPAND);
	ctrlSizer->Add(xMaxCtrl, 0, wxEXPAND);
	ctrlSizer->Add(new wxStaticText(sidebar, wxID_ANY, "Y Range:"), 0, wxTOP, 10);
	ctrlSizer->Add(yMinCtrl, 0, wxEXPAND);
	ctrlSizer->Add(yMaxCtrl, 0, wxEXPAND);
	ctrlSizer->Add(new wxStaticText(sidebar, wxID_ANY, "Internal Nodes:"), 0, wxTOP, 10);
	ctrlSizer->Add(gridSizeCtrl, 0, wxEXPAND);

	ctrlSizer->Add(new wxStaticText(sidebar, wxID_ANY, "Boundary Function:"), 0, wxTOP, 10);
	boundaryFuncCtrl = new wxTextCtrl(sidebar, wxID_ANY, "");
	boundaryFuncCtrl->SetToolTip("Use x and y as variables. Supported: +,-,*,/,^,sin,cos,exp,sqrt,abs");
	ctrlSizer->Add(boundaryFuncCtrl, 0, wxEXPAND);

	ctrlSizer->Add(new wxStaticText(sidebar, wxID_ANY, "Epsilon:"), 0, wxTOP, 10);
	epsilonCtrl = new wxTextCtrl(sidebar, wxID_ANY, "");
	epsilonCtrl->SetToolTip("Convergence tolerance for stopping iterations");
	ctrlSizer->Add(epsilonCtrl, 0, wxEXPAND);

	wxString methods[] = { "1. Liebmann's Method", "2. Thomas's Method" };
	methodBox = new wxRadioBox(sidebar, wxID_ANY, "Method", wxDefaultPosition,
		wxDefaultSize, 2, methods, 1, wxRA_SPECIFY_COLS);
	ctrlSizer->Add(methodBox, 0, wxTOP | wxEXPAND, 15);

	wxButton* solveBtn = new wxButton(sidebar, wxID_ANY, "Run Solver");
	solveBtn->SetBackgroundColour(wxColour(0, 120, 215));
	solveBtn->SetForegroundColour(*wxWHITE);
	ctrlSizer->Add(solveBtn, 0, wxTOP | wxEXPAND, 20);
	sidebar->SetSizer(ctrlSizer);

	notebook = new wxNotebook(mainPanel, wxID_ANY);
	canvas = new wxPanel(notebook);
	canvas->SetBackgroundColour(*wxWHITE);
	notebook->AddPage(canvas, "Visual Grid");

	matrixPanel = new wxTextCtrl(notebook, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
	matrixPanel->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	notebook->AddPage(matrixPanel, "Matrices");

	mainSizer->Add(sidebar, 0, wxEXPAND | wxALL, 5);
	mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
	mainPanel->SetSizer(mainSizer);

	solveBtn->Bind(wxEVT_BUTTON, &PDESolverFrame::OnSolve, this);
	canvas->Bind(wxEVT_PAINT, &PDESolverFrame::OnPaint, this);
	canvas->Bind(wxEVT_MOUSEWHEEL, &PDESolverFrame::OnMouseWheel, this);
	canvas->Bind(wxEVT_MOTION, &PDESolverFrame::OnMouseMotion, this);
	Bind(wxEVT_CHAR_HOOK, &PDESolverFrame::OnKeyDown, this);
}

// --- Input accessors ---

double PDESolverFrame::GetXMin() const {
	double v = 0.0; xMinCtrl->GetValue().ToDouble(&v); return v;
}
double PDESolverFrame::GetXMax() const {
	double v = 0.0; xMaxCtrl->GetValue().ToDouble(&v); return v;
}
double PDESolverFrame::GetYMin() const {
	double v = 0.0; yMinCtrl->GetValue().ToDouble(&v); return v;
}
double PDESolverFrame::GetYMax() const {
	double v = 0.0; yMaxCtrl->GetValue().ToDouble(&v); return v;
}
int      PDESolverFrame::GetGridSize()    const { return gridSizeCtrl->GetValue(); }
wxString PDESolverFrame::GetBoundaryFunc() const { return boundaryFuncCtrl->GetValue(); }
double PDESolverFrame::GetEpsilon() const {
	double v = 0.01; epsilonCtrl->GetValue().ToDouble(&v); return v;
}
int PDESolverFrame::GetMethod() const { return methodBox->GetSelection(); }

// --- Output setters ---

void PDESolverFrame::SetMatrixOutput(const wxString& text) { matrixPanel->SetValue(text); }
void PDESolverFrame::RefreshCanvas() { canvas->Refresh(); }

// --- Event handlers ---

void PDESolverFrame::OnSolve(wxCommandEvent&) { controller->RunSolver(); }

void PDESolverFrame::OnKeyDown(wxKeyEvent& event) {
	if (event.ControlDown()) {
		int key = event.GetKeyCode();
		if (key == '+' || key == '=') zoomFactor *= 1.2;
		if (key == '-') zoomFactor /= 1.2;
		canvas->Refresh();
	}
	event.Skip();
}

void PDESolverFrame::OnMouseWheel(wxMouseEvent& event) {
	if (event.ControlDown()) {
		if (event.GetWheelRotation() > 0) zoomFactor *= 1.2;
		else                               zoomFactor /= 1.2;
		canvas->Refresh();
	}
	else {
		event.Skip();
	}
}

void PDESolverFrame::OnMouseMotion(wxMouseEvent& event) {
	const PDEModel* model = controller->GetModel();
	int w, h_c; canvas->GetSize(&w, &h_c);
	int cx = w / 2, cy = h_c / 2;

	wxPoint mouse = event.GetPosition();
	int oldI = hoverI, oldJ = hoverJ;
	hoverI = hoverJ = -1;

	double minDist = 15.0;
	int nx = model->GetNx(), ny = model->GetNy();
	double xMin = model->GetXMin(), yMin = model->GetYMin();
	double hx = model->GetHx(), hy = model->GetHy();

	for (int j = 0; j < ny; j++) {
		for (int i = 0; i < nx; i++) {
			int px = cx + (int)((xMin + i * hx) * zoomFactor);
			int py = cy - (int)((yMin + j * hy) * zoomFactor);
			double dist = sqrt(pow(mouse.x - px, 2) + pow(mouse.y - py, 2));
			if (dist < minDist) { minDist = dist; hoverI = i; hoverJ = j; }
		}
	}

	if (oldI != hoverI || oldJ != hoverJ) canvas->Refresh();
}

void PDESolverFrame::OnPaint(wxPaintEvent&) {
	const PDEModel* model = controller->GetModel();
	if (model->GetNx() == 0) return;

	wxPaintDC dc(canvas);
	int w, h_c; canvas->GetSize(&w, &h_c);
	int cx = w / 2, cy = h_c / 2;

	int nx = model->GetNx(), ny = model->GetNy();
	double xMin = model->GetXMin(), xMax = model->GetXMax();
	double yMin = model->GetYMin(), yMax = model->GetYMax();
	double hx = model->GetHx(), hy = model->GetHy();
	const auto& u = model->GetSolution();

	// Draw axes
	dc.SetPen(wxPen(*wxBLACK, 2));
	dc.DrawLine(20, cy, w - 20, cy);
	dc.DrawLine(cx, 20, cx, h_c - 20);

	// Arrows
	dc.SetBrush(*wxBLACK_BRUSH);
	wxPoint xArrow[3] = { {w - 20, cy}, {w - 30, cy - 5}, {w - 30, cy + 5} };
	dc.DrawPolygon(3, xArrow);
	wxPoint yArrow[3] = { {cx, 20}, {cx - 5, 30}, {cx + 5, 30} };
	dc.DrawPolygon(3, yArrow);

	dc.SetTextForeground(*wxBLACK);
	dc.DrawText("X", w - 15, cy + 5);
	dc.DrawText("Y", cx + 5, 10);

	// Tick marks
	dc.SetPen(wxPen(*wxBLACK, 1));
	for (double x = ceil(xMin); x <= floor(xMax); x += 1.0) {
		int px = cx + (int)(x * zoomFactor);
		dc.DrawLine(px, cy - 5, px, cy + 5);
		if (fabs(x) > 0.01) dc.DrawText(wxString::Format("%.0f", x), px - 8, cy + 8);
	}
	for (double y = ceil(yMin); y <= floor(yMax); y += 1.0) {
		int py = cy - (int)(y * zoomFactor);
		dc.DrawLine(cx - 5, py, cx + 5, py);
		if (fabs(y) > 0.01) dc.DrawText(wxString::Format("%.0f", y), cx + 8, py - 8);
	}
	dc.DrawText("O", cx - 15, cy + 5);

	// Grid points
	for (int j = 0; j < ny; j++) {
		for (int i = 0; i < nx; i++) {
			int px = cx + (int)((xMin + i * hx) * zoomFactor);
			int py = cy - (int)((yMin + j * hy) * zoomFactor);
			bool isBoundary = (i == 0 || i == nx - 1 || j == 0 || j == ny - 1);
			bool isHovered = (i == hoverI && j == hoverJ);

			if (isHovered) {
				dc.SetBrush(*wxYELLOW_BRUSH);
				dc.DrawCircle(px, py, 6);
			}
			else {
				dc.SetBrush(isBoundary ? *wxRED_BRUSH : *wxGREEN_BRUSH);
				dc.DrawCircle(px, py, 4);
			}

			dc.SetTextForeground(*wxBLUE);
			dc.DrawText(wxString::Format("%.2f", u[j][i]), px + 5, py - 15);

			if (isHovered) {
				wxString label = isBoundary
					? wxString::Format("Boundary [%d,%d]", i, j)
					: wxString::Format("u%d%d", j, i);

				wxSize sz = dc.GetTextExtent(label);
				dc.SetBrush(wxBrush(wxColour(255, 255, 200, 220)));
				dc.SetPen(wxPen(*wxBLACK, 1));
				dc.DrawRectangle(px + 8, py + 5, sz.GetWidth() + 4, sz.GetHeight() + 2);
				dc.SetTextForeground(*wxBLACK);
				dc.DrawText(label, px + 10, py + 6);
			}
		}
	}
}