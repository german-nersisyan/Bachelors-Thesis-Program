#pragma once
#include <wx/wx.h>
#include "PDESolverController.h"

class MyApp : public wxApp {
public:
	bool OnInit() override;

private:
	PDESolverController controller;
};