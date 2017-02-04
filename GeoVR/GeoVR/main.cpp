#include "VREngine.h"


int main(int argc, char* argv[]) {
	VREngine* MainApp = new VREngine();

	if (!MainApp->Init()) {
		MainApp->Shutdown();
		return 1;
	}

	MainApp->Loop();
	MainApp->Shutdown();

	return 0;
}