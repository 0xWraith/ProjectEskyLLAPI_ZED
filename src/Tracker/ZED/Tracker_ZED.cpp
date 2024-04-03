#include "Tracker_ZED.h"
#include <sstream>

TrackerObject::TrackerObject(FuncCallBack _callback) {
	callbackInstance = _callback;
}

void TrackerObject::stopTracking() {

}

void TrackerObject::tracking(bool use_localization) {

	sl::Camera zed;
	sl::InitParameters init_params = configureCamera(sl::RESOLUTION::HD720, sl::FLIP_MODE::AUTO, sl::DEPTH_MODE::PERFORMANCE, sl::COORDINATE_SYSTEM::LEFT_HANDED_Y_UP, sl::UNIT::METER);
	sl::ERROR_CODE open_result = zed.open(init_params);

	if (open_result != sl::ERROR_CODE::SUCCESS) {
        std::stringstream ss;
        ss << "Error " << open_result << ", exit program.";
		Debug::Log(ss.str());
		return;
	}

	while (true) {

	}
}

sl::InitParameters TrackerObject::configureCamera(sl::RESOLUTION resolution, sl::FLIP_MODE flip, sl::DEPTH_MODE depth_mode, sl::COORDINATE_SYSTEM coordinate_system, sl::UNIT coordinate_units) {
	sl::InitParameters parameters;
	parameters.camera_resolution = resolution;
	parameters.camera_fps = 0; // Will use the max fps the camera can do on this resolution
	parameters.camera_image_flip = flip;
	parameters.depth_mode = depth_mode;
	parameters.coordinate_system = coordinate_system;
	parameters.coordinate_units = coordinate_units;
	return parameters;
}
