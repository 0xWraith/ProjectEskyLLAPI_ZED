#include "Tracker_ZED.h"
#include <sstream>

TrackerObject::TrackerObject(FuncCallBack _callback) {
	callbackInstance = _callback;
}

void TrackerObject::stopTracking() {

}

void TrackerObject::tracking(bool use_localization) {

	sl::Camera zed;
    sl::POSITIONAL_TRACKING_STATE tracking_state = sl::POSITIONAL_TRACKING_STATE::OFF;

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

    chrono::high_resolution_clock::time_point last_time_stamp = chrono::high_resolution_clock::now();
    

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
        err = zed.grab();
        if (err == sl::ERROR_CODE::SUCCESS) {
            this->process_camera_position(zed, &tracking_state);
            if (spatial_mapping_succesfully_started) {
                this->process_spatial_mapping(zed, &last_time_stamp);
            }
        } else {
            std::stringstream ss;
            ss << "Error " << err << ", while grabbing frame";
            Debug::Log(ss.str());
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

void TrackerObject::process_camera_position(sl::Camera& zed, sl::POSITIONAL_TRACKING_STATE* tracking_state) {
    sl::Pose camera_position;
    sl::POSITIONAL_TRACKING_STATE current_tracking_state = zed.getPosition(camera_position, sl::REFERENCE_FRAME::WORLD);
    if (current_tracking_state != *tracking_state) {
        *tracking_state = current_tracking_state;
        if (*tracking_state == sl::POSITIONAL_TRACKING_STATE::OK) {
            callback_localization(0, 1);
        }
        std::stringstream ss;
        ss << "Tracking state: " << *tracking_state;
        Debug::Log(ss.str());
    } //????
    sl::Translation camera_translation = camera_position.getTranslation();
    sl::float3 camera_euler_angles = camera_position.getEulerAngles();

    this->camera_position[0] = camera_translation.x;
    this->camera_position[1] = camera_translation.y;
    this->camera_position[2] = camera_translation.z;
    this->camera_position[3] = camera_euler_angles.x;
    this->camera_position[4] = camera_euler_angles.y;
    this->camera_position[5] = camera_euler_angles.z;
}

void TrackerObject::process_spatial_mapping(sl::Camera& zed, chrono::high_resolution_clock::time_point* last_time_stamp) {

    if (stop_spacial_mapping) {
        zed.disableSpatialMapping();
        stop_spacial_mapping = spatial_mapping_succesfully_started = false;
        Debug::Log("Spatial Mapping Disabled");
    }

    sl::SPATIAL_MAPPING_STATE mapping_state = zed.getSpatialMappingState();
    if (mapping_state == sl::SPATIAL_MAPPING_STATE::NOT_ENABLED || mapping_state == sl::SPATIAL_MAPPING_STATE::NOT_ENOUGH_MEMORY || mapping_state == sl::SPATIAL_MAPPING_STATE::FPS_TOO_LOW) {
        std::stringstream ss;
        ss << "Error " << mapping_state << ", while spatial mapping";
        Debug::Log(ss.str());
    } else {
        if (mapping_state == sl::SPATIAL_MAPPING_STATE::INITIALIZING) {
            Debug::Log("Spatial Mapping Initializing");
        } else if (mapping_state == sl::SPATIAL_MAPPING_STATE::OK) {
            Debug::Log("Spatial Mapping OK");

            auto now = chrono::high_resolution_clock::now();
            auto time_interval = chrono::duration_cast<chrono::milliseconds>(now - *last_time_stamp).count();

            if (time_interval > TrackerObject::REQUEST_CHUNKS_TIME_INTERVAL && request_new_chunks) {
                *last_time_stamp = now;
                request_new_chunks = false;
                zed.requestSpatialMapAsync();
            }
            if (zed.getSpatialMapRequestStatusAsync() != sl::ERROR_CODE::SUCCESS) {
                Debug::Log("Error while requesting spatial map");
                return;
            }
            if (mesh_callback != nullptr && mesh_complete_callback != nullptr) {
                sl::Mesh map;
                sl::ERROR_CODE err = zed.retrieveSpatialMapAsync(map);
                if (err != sl::ERROR_CODE::SUCCESS) {
                    std::stringstream ss;
                    ss << "Error " << err << ", while retrieving spatial map";
                    Debug::Log(ss.str());
                    return;
                }
                Debug::Log("Spatial Map Retrieved, chunk count: " + std::to_string(map.chunks.size()));

                std::vector<float> uvs;
                std::vector<float> normals;
                std::vector<float> vertices;
                std::vector<int> triangle_indicies;

                for (size_t i = 0, size = map.chunks.size(); i < size; i++) {
                    sl::Chunk chunk = map.chunks[i];
                    for (size_t j = 0, size = chunk.vertices.size(); j < size; j++) {
                        sl::float3 vertex = chunk.vertices[j];
                        vertices.push_back(vertex.x);
                        vertices.push_back(vertex.y);
                        vertices.push_back(vertex.z);
                    }
                    for (size_t j = 0, size = chunk.triangles.size(); j < size; j++) {
                        sl::uint3 triangle = chunk.triangles[j];
                        triangle_indicies.push_back(triangle.x);
                        triangle_indicies.push_back(triangle.y);
                        triangle_indicies.push_back(triangle.z);
                    }
                    for (size_t j = 0, size = chunk.normals.size(); j < size; j++) {
                        sl::float3 normal = chunk.normals[j];
                        normals.push_back(normal.x);
                        normals.push_back(normal.y);
                        normals.push_back(normal.z);
                    }
                    for (size_t j = 0, size = chunk.uv.size(); j < size; j++) {
                        sl::float2 uv = chunk.uv[j];
                        uvs.push_back(uv.x);
                        uvs.push_back(uv.y);
                    }
                    if (vertices.size() > 0 && chunk.has_been_updated) {
                        mesh_callback(i, vertices.data(), vertices.size(), normals.data(), normals.size(), uvs.data(), uvs.size(), triangle_indicies.data(), triangle_indicies.size());
                    }
                }
                mesh_complete_callback();
            }
        }
    }
}
