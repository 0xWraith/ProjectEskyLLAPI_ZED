#include "common_header.h"
#include <stdio.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Tracker_ZED.h" 

#define DLL_EXPORT __declspec(dllexport)

void trackerThread(bool use_localization);

#ifdef __cplusplus
extern "C" {
#endif
    TrackerObject* to = nullptr;
    ID3D11Device* device = nullptr;
    std::thread* trackerThreadPtr = nullptr;
    
    DLL_EXPORT void SaveOriginPose() {
        Debug::Log("Saving Origin Pose");
    }

    DLL_EXPORT float* GetLatestPose() {
        Debug::Log("Getting Latest Pose");
        return nullptr;
    }

    DLL_EXPORT void InitializeTrackerObject() {
        Debug::Log("Initializing Tracker Object");
        if (to == nullptr) {
            to = new TrackerObject(callbackInstance);
            Debug::Log("Tracker Object Initialized");
        }
    }

    DLL_EXPORT void StartTrackerThread(bool use_localization) {
        Debug::Log("Starting Tracker Thread");
        std::cout << "Arguments: " << use_localization << std::endl;
        if (to == nullptr) {
            Debug::Log("StartTrackerThread: Tracker hasn't been initialized");
            return;
        }
        trackerThreadPtr = new std::thread(trackerThread, use_localization);
    }

    DLL_EXPORT void StopTrackers() {
        Debug::Log("Stopping Trackers");
    }

    DLL_EXPORT void SetObjectPoseInLocalizedMap(const char* object_id, float tx, float ty, float tz, float qx, float qy, float qz, float qw) {
        Debug::Log("Setting Object Pose In Localized Map");
        std::cout << "Arguments: " << object_id << " " << tx << " " << ty << " " << tz << " " << qx << " " << qy << " " << qz << " " << qw << std::endl;
    }

    DLL_EXPORT void ObtainObjectPoseInLocalizedMap(const char* object_id) {
        Debug::Log("Obtaining Object Pose In Localized Map");
        std::cout << "Arguments: " << object_id << std::endl;
    }

    DLL_EXPORT void ObtainMap() {
        Debug::Log("Obtaining Map");
    }

    DLL_EXPORT void HookDeviceToZed() {
        Debug::Log("Hooking Device To Zed");
    }

    DLL_EXPORT void SetTextureInitializedCallback(TrackerObject::FuncTextureInitializedCallback callback) {
        Debug::Log("Setting Texture Initialized Callback");
    }

    DLL_EXPORT void RegisterMeshCompleteCallback(TrackerObject::FuncMeshCompleteCallback callback) { 
        Debug::Log("Registering Mesh Complete Callback");
    }
    
    DLL_EXPORT void RegisterMeshCallback(TrackerObject::FuncMeshCallback my_callback) {
        Debug::Log("Registering Mesh Callback");
    }

    DLL_EXPORT void SetMapData(unsigned char* input_data, int Length) {
        Debug::Log("Setting Map Data");
    }

    DLL_EXPORT void StartSpatialMapping(int chunk_sizes) {
        Debug::Log("Starting Spatial Mapping");
        std::cout << "Arguments: " << chunk_sizes << std::endl;
    }

    DLL_EXPORT void StopSpatialMapping(int chunk_sizes) {
        Debug::Log("Stopping Spatial Mapping");
        std::cout << "Arguments: " << chunk_sizes << std::endl;
    }

    DLL_EXPORT void SetRenderTexturePointer(void* texture_handle) {
        Debug::Log("Setting Render Texture Pointer");
    }

    DLL_EXPORT void CompletedMeshUpdate() {
        Debug::Log("Completed Mesh Update");
    }

    DLL_EXPORT void RegisterDebugCallback(FuncCallBack cb);
    DLL_EXPORT void RegisterBinaryMapCallback(TrackerObject::FuncCallBack3 cb);
    DLL_EXPORT void RegisterObjectPoseCallback(TrackerObject::FuncCallBack4 cb);
    DLL_EXPORT void RegisterLocalizationCallback(TrackerObject::FuncCallBack2 cb);
}

static void UNITY_INTERFACE_API OnRenderEvent(int event_id) {
    Debug::Log("On Render Event");
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    Debug::Log("Getting Render Event Function");
    std::cout << "Getting Render Event Function" << std::endl;
    return OnRenderEvent;
}

void RegisterDebugCallback(FuncCallBack cb) { 
    Debug::Log("Registering Debug Callback");
    callbackInstance = cb;
}

void RegisterLocalizationCallback(TrackerObject::FuncCallBack2 callback) {
    Debug::Log("Registering Localization Callback");
    if (to == nullptr) {
        Debug::Log("RegisterLocalizationCallback: Tracker hasn't been initialized");
        return;
    }
    to->callback_localization = callback;
}

void RegisterBinaryMapCallback(TrackerObject::FuncCallBack3 callback) {
    Debug::Log("Registering Binary Map Callback");
    if (to == nullptr) {
        Debug::Log("RegisterBinaryMapCallback: Tracker hasn't been initialized");
        return;
    }
    to->callback_binary_map = callback;
}

void RegisterObjectPoseCallback(TrackerObject::FuncCallBack4 callback) {
    Debug::Log("Registering Object Pose Callback");
    if (to == nullptr) {
        Debug::Log("RegisterObjectPoseCallback: Tracker hasn't been initialized");
        return;
    }
    to->callback_object_pose_received = callback;
}

void trackerThread(bool use_localization) {
    if (to == nullptr) {
        Debug::Log("Tracker hasn't been initialized");
        return;
    }
    Debug::Log("Tracker Thread");
    to->tracking(use_localization);
    delete to;
    to = nullptr;
    Debug::Log("Tracker Thread Finished");
}
