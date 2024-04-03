#include "common_header.h"
#include <stdio.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Tracker_ZED.h" 

#define DLL_EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

    DLL_EXPORT void SaveOriginPose() {
        Debug::Log("Saving Origin Pose");
        std::cout << "Saving Origin Pose" << std::endl;
    }

    DLL_EXPORT float* GetLatestPose() {
        Debug::Log("Getting Latest Pose");
        std::cout << "Getting Latest Pose" << std::endl;
        return nullptr;
    }

    DLL_EXPORT void InitializeTrackerObject() {
        Debug::Log("Initializing Tracker Object");
        std::cout << "Initializing Tracker Object" << std::endl;
    }

    DLL_EXPORT void StartTrackerThread(bool useLocalization) {
        Debug::Log("Starting Tracker Thread");
        std::cout << "Starting Tracker Thread" << std::endl;
        std::cout << "Arguments: " << useLocalization << std::endl;
    }

    DLL_EXPORT void StopTrackers() {
        Debug::Log("Stopping Trackers");
        std::cout << "Stopping Trackers" << std::endl;
    }

    DLL_EXPORT void SetObjectPoseInLocalizedMap(const char* objectID, float tx, float ty, float tz, float qx, float qy, float qz, float qw) {
        Debug::Log("Setting Object Pose In Localized Map");
        std::cout << "Setting Object Pose In Localized Map" << std::endl;
        std::cout << "Arguments: " << objectID << " " << tx << " " << ty << " " << tz << " " << qx << " " << qy << " " << qz << " " << qw << std::endl;
    }

    DLL_EXPORT void ObtainObjectPoseInLocalizedMap(const char* objectID) {
        Debug::Log("Obtaining Object Pose In Localized Map");
        std::cout << "Obtaining Object Pose In Localized Map" << std::endl;
        std::cout << "Arguments: " << objectID << std::endl;
    }

    DLL_EXPORT void ObtainMap() {
        Debug::Log("Obtaining Map");
        std::cout << "Obtaining Map" << std::endl;
    }

    DLL_EXPORT void HookDeviceToZed() {
        Debug::Log("Hooking Device To Zed");
        std::cout << "Hooking Device To Zed" << std::endl;
    }

    DLL_EXPORT void SetTextureInitializedCallback(TrackerObject::FuncTextureInitializedCallback callback) {
        Debug::Log("Setting Texture Initialized Callback");
        std::cout << "Setting Texture Initialized Callback" << std::endl;
    }

    DLL_EXPORT void RegisterMeshCompleteCallback(TrackerObject::FuncMeshCompleteCallback callback) { 
        Debug::Log("Registering Mesh Complete Callback");
        std::cout << "Registering Mesh Complete Callback" << std::endl;
    }
    
    DLL_EXPORT void RegisterMeshCallback(TrackerObject::FuncMeshCallback myCallback) {
        Debug::Log("Registering Mesh Callback");
        std::cout << "Registering Mesh Callback" << std::endl;
    }

    DLL_EXPORT void SetMapData(unsigned char* inputData, int Length) {
        Debug::Log("Setting Map Data");
        std::cout << "Setting Map Data" << std::endl;
    }

    DLL_EXPORT void StartSpatialMapping(int chunkSizes) {
        Debug::Log("Starting Spatial Mapping");
        std::cout << "Starting Spatial Mapping" << std::endl;
        std::cout << "Arguments: " << chunkSizes << std::endl;
    }

    DLL_EXPORT void StopSpatialMapping(int chunkSizes) {
        Debug::Log("Stopping Spatial Mapping");
        std::cout << "Stopping Spatial Mapping" << std::endl;
        std::cout << "Arguments: " << chunkSizes << std::endl;
    }

    DLL_EXPORT void SetRenderTexturePointer(void* textureHandle) {
        Debug::Log("Setting Render Texture Pointer");
        std::cout << "Setting Render Texture Pointer" << std::endl;
    }

    DLL_EXPORT void CompletedMeshUpdate() {
        Debug::Log("Completed Mesh Update");
        std::cout << "Completed Mesh Update" << std::endl;
    }

    DLL_EXPORT void RegisterDebugCallback(FuncCallBack cb);
    DLL_EXPORT void RegisterBinaryMapCallback(TrackerObject::FuncCallBack3 cb);
    DLL_EXPORT void RegisterObjectPoseCallback(TrackerObject::FuncCallBack4 cb);
    DLL_EXPORT void RegisterLocalizationCallback(TrackerObject::FuncCallBack2 cb);


}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
    Debug::Log("On Render Event");
    std::cout << "On Render Event" << std::endl;
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    Debug::Log("Getting Render Event Function");
    std::cout << "Getting Render Event Function" << std::endl;
    return OnRenderEvent;
}

void RegisterDebugCallback(FuncCallBack cb) { 
    Debug::Log("Registering Debug Callback");
    std::cout << "Registering Debug Callback" << std::endl;
    
    callbackInstance = cb;
}

void RegisterLocalizationCallback(TrackerObject::FuncCallBack2 cb) {
    Debug::Log("Registering Localization Callback");
    std::cout << "Registering Localization Callback" << std::endl;
}
void RegisterBinaryMapCallback(TrackerObject::FuncCallBack3 cb) {
    Debug::Log("Registering Binary Map Callback");
    std::cout << "Registering Binary Map Callback" << std::endl;
}

void RegisterObjectPoseCallback(TrackerObject::FuncCallBack4 cb) {
    Debug::Log("Registering Object Pose Callback");
    std::cout << "Registering Object Pose Callback" << std::endl;
}
