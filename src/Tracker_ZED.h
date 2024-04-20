
//#define VS_CODE

#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <math.h>
#include <stack>
#include <float.h>
#include <sstream>
#include "common_header.h"
#ifdef VS_CODE
#include "C:/Program Files (x86)/ZED SDK/include/sl/Camera.hpp"
#else
#include <sl/Camera.hpp>
#endif
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"
#include "IUnityInterface.h"
#include "IUnityGraphics.h"
class TrackerObject {
public:

    static const int REQUEST_CHUNKS_TIME_INTERVAL = 100;

    typedef void(*FuncTextureInitializedCallback)(size_t texture_width, size_t texture_height, size_t texture_channels, float v_fov);
    FuncTextureInitializedCallback texture_initialized_callback = nullptr;
    
    typedef void(*FuncMeshCompleteCallback)();
    FuncMeshCompleteCallback mesh_complete_callback = nullptr;

    typedef void(*FuncMeshCallback)(size_t chunk_id, float* vertices, size_t vertices_length, float* normals, size_t normals_length, float* uvs, size_t uvs_length, int* triangle_indicies, size_t triangle_indicies_length);
    FuncMeshCallback mesh_callback = nullptr;

    typedef void(*FuncCallBack2)(int tracker_id, int localization_delegate); 
    FuncCallBack2 callback_localization = nullptr;

    typedef void(*FuncCallBack3)(int tracker_id, unsigned char* binary_data, int Length);
    FuncCallBack3 callback_binary_map = nullptr;

    typedef void(*FuncCallBack4)(int tracker_id, string objectID, float tx, float ty, float tz, float qx, float qy, float qz, float qw);
    FuncCallBack4 callback_object_pose_received = nullptr;

    ID3D11Device* device = nullptr;
    ID3D11Texture2D* d3d_texture = nullptr;
    
    bool thread_alive = true;

    bool request_new_chunks = true;
    bool stop_spacial_mapping = false;
    bool start_spacial_mapping = false;

    bool lock_image = false;

    float spatial_mapping_range = 1.0f;
    float spatial_mapping_resolution = 0.08f;

    float camera_position[6] = { 0, 0, 0, 0, 0, 0 };

    TrackerObject(FuncCallBack callback);
    ~TrackerObject();

    void stopTracking();
    void tracking(bool use_localization);
    void update_camera_texture_gpu();
    void process_grab_map();

private: 
    size_t texture_width = 0;
    size_t texture_height = 0;
    size_t texture_channels = 0;
    sl::Mat current_image;

    bool grab_map = false;
    bool spatial_mapping_succesfully_started = false;

    sl::PositionalTrackingParameters configure_positional_tracking_parameters(bool enable_pose_smoothing, sl::POSITIONAL_TRACKING_MODE mode);
    sl::InitParameters configure_init_parameters(sl::RESOLUTION resolution, sl::FLIP_MODE flip, sl::DEPTH_MODE depth_mode, sl::COORDINATE_SYSTEM coordinate_system, sl::UNIT coordinate_units);
    sl::SpatialMappingParameters configure_spatial_mapping_parameters(float resolution_meter, bool use_chunk_only, float range_meter, bool save_texture, sl::SpatialMappingParameters::SPATIAL_MAP_TYPE map_type);

    bool file_exist(const char* fileName);

    void process_retreived_image(sl::Camera& zed);
    void process_camera_position(sl::Camera& zed, sl::POSITIONAL_TRACKING_STATE* tracking_state);
    void process_spatial_mapping(sl::Camera& zed, chrono::high_resolution_clock::time_point* last_time_stamp);
};