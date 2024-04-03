
#define VS_CODE

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
    typedef void(*FuncTextureInitializedCallback)(int texture_width, int texture_height, int texture_channels, float v_fov);
    FuncTextureInitializedCallback texture_initialized_callback = nullptr;
    
    typedef void(*FuncMeshCompleteCallback)();
    FuncMeshCompleteCallback mesh_complete_callback = nullptr;

    typedef void(*FuncMeshCallback)(int chunk_id, float* vertices, int vertices_length, float* normals, int normals_length, float* uvs, int uvs_length, int* triangle_indicies, int triangle_indicies_length);
    FuncMeshCallback mesh_callback = nullptr;

    typedef void(*FuncCallBack2)(int tracker_id, int localization_delegate); 
    FuncCallBack2 callback_localization = nullptr;

    typedef void(*FuncCallBack3)(int tracker_id, unsigned char* binary_data, int Length);
    FuncCallBack3 callback_binary_map = nullptr;

    typedef void(*FuncCallBack4)(int tracker_id, string objectID, float tx, float ty, float tz, float qx, float qy, float qz, float qw);
    FuncCallBack4 callback_object_pose_received = nullptr;


    sl::Mat current_image;
    sl::Mat current_gpu_image;
    ID3D11Device* device = nullptr;
    
    bool thread_alive = true;
    bool start_spacial_mapping = false;

    TrackerObject(FuncCallBack callback);

    void stopTracking();
    void tracking(bool use_localization);

private: 

    bool spatial_mapping_succesfully_started = false;

    sl::PositionalTrackingParameters configure_positional_tracking_parameters(bool enable_pose_smoothing, sl::POSITIONAL_TRACKING_MODE mode);
    sl::InitParameters configure_init_parameters(sl::RESOLUTION resolution, sl::FLIP_MODE flip, sl::DEPTH_MODE depth_mode, sl::COORDINATE_SYSTEM coordinate_system, sl::UNIT coordinate_units);
    sl::SpatialMappingParameters configure_spatial_mapping_parameters(float resolution_meter, bool use_chunk_only, float range_meter, bool save_texture, sl::SpatialMappingParameters::SPATIAL_MAP_TYPE map_type);
};