#include "Tracker_ZED.h"
#include <sstream>

TrackerObject::TrackerObject(FuncCallBack _callback) {
	callbackInstance = _callback;
}

void TrackerObject::stopTracking() {

}

void TrackerObject::tracking(bool use_localization) {

	sl::Camera zed;
	sl::InitParameters init_params = configure_init_parameters(sl::RESOLUTION::HD720, sl::FLIP_MODE::AUTO, sl::DEPTH_MODE::PERFORMANCE, sl::COORDINATE_SYSTEM::LEFT_HANDED_Y_UP, sl::UNIT::METER);
	sl::ERROR_CODE err = zed.open(init_params);

	if (err != sl::ERROR_CODE::SUCCESS) {
        std::stringstream ss;
        ss << "Error " << err << ", while opening the ZED camera, exit program.";
		Debug::Log(ss.str());
		return;
	}
    Debug::Log("ZED Camera Opened");

	sl::PositionalTrackingParameters tracking_params = configure_positional_tracking_parameters(true, sl::POSITIONAL_TRACKING_MODE::STANDARD);
	err = zed.enablePositionalTracking(tracking_params);
	if (err != sl::ERROR_CODE::SUCCESS) {
		std::stringstream ss;
		ss << "Error " << err << ", while enabling positional tracking, exit program.";
		Debug::Log(ss.str());
		zed.close();
		return;
	}
	Debug::Log("Positional Tracking Enabled");

	while (thread_alive) {
        if (start_spacial_mapping) {
            start_spacial_mapping = false;
            sl::SpatialMappingParameters spatial_mapping_params = configure_spatial_mapping_parameters(
                sl::SpatialMappingParameters::get(sl::SpatialMappingParameters::MAPPING_RESOLUTION::MEDIUM),
                false,
                sl::SpatialMappingParameters::get(sl::SpatialMappingParameters::MAPPING_RANGE::MEDIUM),
                false,
                sl::SpatialMappingParameters::SPATIAL_MAP_TYPE::MESH);
                
            err = zed.enableSpatialMapping(spatial_mapping_params);
            if (err != sl::ERROR_CODE::SUCCESS) {
                std::stringstream ss;
                ss << "Error " << err << ", while enabling spatial mapping";
                Debug::Log(ss.str());
            }
            else {
                spatial_mapping_succesfully_started = true;
                Debug::Log("Spatial Mapping Enabled");
            }
        }

        if (zed.grab() == sl::ERROR_CODE::SUCCESS) {

        }
	}
}

sl::InitParameters TrackerObject::configure_init_parameters(sl::RESOLUTION resolution, sl::FLIP_MODE flip, sl::DEPTH_MODE depth_mode, sl::COORDINATE_SYSTEM coordinate_system, sl::UNIT coordinate_units) {
	Debug::Log("Configuring Init Parameters");

	sl::InitParameters parameters;
	parameters.camera_resolution = resolution;
	parameters.camera_fps = 0; // Will use the max fps the camera can do on this resolution
	parameters.camera_image_flip = flip;
	parameters.depth_mode = depth_mode;
	parameters.coordinate_system = coordinate_system;
	parameters.coordinate_units = coordinate_units;
	return parameters;
}

sl::PositionalTrackingParameters TrackerObject::configure_positional_tracking_parameters(bool enable_pose_smoothing, sl::POSITIONAL_TRACKING_MODE mode) {
	Debug::Log("Configuring Positional Tracking Parameters");

	sl::PositionalTrackingParameters parameters;
	parameters.enable_pose_smoothing = enable_pose_smoothing;
	parameters.mode = mode;
	parameters.area_file_path = "zed_area_file.area";
	return parameters;
}

sl::SpatialMappingParameters TrackerObject::configure_spatial_mapping_parameters(float resolution_meter, bool use_chunk_only, float range_meter, bool save_texture, sl::SpatialMappingParameters::SPATIAL_MAP_TYPE map_type) {
	Debug::Log("Configuring Spatial Mapping Parameters");

	sl::SpatialMappingParameters parameters;
	parameters.resolution_meter = resolution_meter;
	parameters.use_chunk_only = use_chunk_only;
	parameters.range_meter = range_meter;
	parameters.save_texture = save_texture;
	parameters.map_type = map_type;
	return parameters;
}
