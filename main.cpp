#include "openvr.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

using namespace vr;

std::string usingWMR;

int bumper = 0;
int start = 0;
int button1 = 0;
int button2 = 0;
int button3 = 0;
int button4 = 0;

int waitForPress() {
	int retVal = 0;
	while (true) {
		for (int i = 0; i < k_unMaxTrackedDeviceCount; i++) {
			if (VRSystem()->GetTrackedDeviceClass(i) != TrackedDeviceClass_Controller) continue;
			VRControllerState_t state;
			if (!VRSystem()->GetControllerState(i, &state, sizeof(state))) continue;

			if (state.ulButtonPressed != 0) {
				for (int j = 0; j < k_EButton_Max; j++) {
					if ((state.ulButtonPressed & ButtonMaskFromId((EVRButtonId)j)) != 0) retVal = j;
				}
			}
			else if (retVal != 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				return retVal;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main() {
	EVRInitError err;
	VR_Init(&err, VRApplication_Utility);

	if (err != VRInitError_None) {
		std::cerr << VR_GetVRInitErrorAsEnglishDescription(err) << "\n";
		return 1;
	}

	std::ofstream binds;
	binds.open("binds.ini");

	if (!binds.is_open()) {
		std::cout << "Failed to open binds.ini!\n";
		VR_Shutdown();
		return 1;
	}

	binds << "[BINDS]\n# for a list of button IDs, take a look at https ://github.com/ValveSoftware/openvr/blob/master/headers/openvr_capi.h#L744\n";

	std::cout << "Are you using Windows MR? (y/n)\n";
	std::getline(std::cin, usingWMR);

	std::cout << "Press the button you would like to use as the bumper (this is not the same as the trigger)\n";
	bumper = waitForPress();
	binds << "BUMPER=" << bumper << "\n";

	std::cout << "Press the button you would like to use as the start button\n";
	start = waitForPress();
	binds << "START=" << start << "\n\n";

	binds << "# we use the touchpad for buttons 1-4 by default\n";

	std::cout << "Press the button you would like to use as button 1 (use/ping in portal 2)\n";
	button1 = waitForPress();
	if (button1 == k_EButton_SteamVR_Touchpad) binds << "BUTTON1=0\n";
	else binds << "BUTTON1=" << button1 << "\n";

	std::cout << "Press the button you would like to use as button 2 (use/ping in portal 2)\n";
	button2 = waitForPress();
	if (button2 == k_EButton_SteamVR_Touchpad) binds << "BUTTON2=0\n";
	else binds << "BUTTON2=" << button2 << "\n";

	std::cout << "Press the button you would like to use as button 3 (jump/gesture in portal 2)\n";
	button3 = waitForPress();
	if (button3 == k_EButton_SteamVR_Touchpad) binds << "BUTTON3=0\n";
	else binds << "BUTTON3=" << button3 << "\n";

	std::cout << "Press the button you would like to use as button 4 (jump/gesture in portal 2)\n";
	button4 = waitForPress();
	if (button4 == k_EButton_SteamVR_Touchpad) binds << "BUTTON4=0\n\n";
	else binds << "BUTTON4=" << button4 << "\n\n";

	binds << "# uncomment to change binding\n";

	if (button1 == k_EButton_SteamVR_Touchpad) binds << "#BUTTON1_USE_TOUCHPAD=0\n";
	else binds << "BUTTON1_USE_TOUCHPAD=0\n";

	if (button2 == k_EButton_SteamVR_Touchpad) binds << "#BUTTON2_USE_TOUCHPAD=0\n";
	else binds << "BUTTON2_USE_TOUCHPAD=0\n";

	if (button3 == k_EButton_SteamVR_Touchpad) binds << "#BUTTON3_USE_TOUCHPAD=0\n";
	else binds << "BUTTON3_USE_TOUCHPAD=0\n";

	if (button4 == k_EButton_SteamVR_Touchpad) binds << "#BUTTON4_USE_TOUCHPAD=0\n\n";
	else binds << "BUTTON4_USE_TOUCHPAD=0\n\n";

	binds << "# set this to 2 if you want to use the Windows MR thumbstick (note that these do not use the IDs found in the link above)\n";
	
	if (usingWMR == "y") binds << "JOYSTICK_AXIS=2\n";
	else binds << "JOYSTICK_AXIS=0\n";

	binds << "TRIGGER_AXIS=1\n";

	binds.close();
	VR_Shutdown();

	return 0;
}