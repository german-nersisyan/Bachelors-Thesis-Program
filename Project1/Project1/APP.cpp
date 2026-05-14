#include "APP.h"
#include "PDESolverFrame.h"

bool MyApp::OnInit() {
	auto* frame = new PDESolverFrame(&controller);
	controller.SetView(frame);
	frame->Show();
	return true;
}