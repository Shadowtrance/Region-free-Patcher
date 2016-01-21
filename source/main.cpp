#include <ctrcommon/input.hpp>
#include <ctrcommon/gpu.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/service.hpp>
#include <ctrcommon/fs.hpp> 
#include <3ds.h>
#include "constants.h"
#include "patches.h"
#include "kernel11.h"
#include "kobjects.h"
#include "menu.h"

using namespace std;

#define log(...) fprintf(stderr, __VA_ARGS__)

bool applyPatches() {
	SaveVersionConstants();

	int result = KernelBackdoor(patchRegionFree);

	HB_FlushInvalidateCache(); // Just to be sure!

	return (result == 0);
}

void clearTop(const bool success) {
	gpuClear();
	gpuViewport(TOP_SCREEN, 0, 0, TOP_WIDTH, TOP_HEIGHT);
	gputOrtho(0, TOP_WIDTH, 0, TOP_HEIGHT, -1, 1);

	if (success)
		gpuClearColor(0, 0xFF, 0, 0xFF);
	else
		gpuClearColor(0xFF, 0xFF, 0, 0xFF);
}

int main(int argc, char **argv) {
	if(!platformInit(argc)) {
		return 0;
	}

	int frameCount = 0;

	const bool patchesSucceeded = applyPatches();
	const int secondsToWait = 2;

	while (platformIsRunning()) {
		clearTop(patchesSucceeded);

		gpuFlushBuffer();
		gpuSwapBuffers(true);

		// Wait for a few seconds before exiting.
		if (frameCount == (secondsToWait * 60))
			break;

		++frameCount;
	}

	platformCleanup();
	return 0;  
}
