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

    static IUnityGraphics* graphics = NULL;
    static IUnityInterfaces* unity_interfaces = NULL;

    DLL_EXPORT float* GetLatestPose() {
        std::stringstream ss;
        ss << "Getting Latest Pose: " << (to == nullptr ? " Tracker Object is Null" : " Tracker Object is not Null");
        Debug::Log(ss.str().c_str());
        return to == nullptr ? new float[6] {0, 0, 0, 0, 0, 0} : to->camera_position;
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
        if (to != nullptr) {
            to->thread_alive = false;
            if (trackerThreadPtr != nullptr) {
                trackerThreadPtr->join();
                delete trackerThreadPtr;
                trackerThreadPtr = nullptr;
                Debug::Log("Tracker Thread Stopped");
            }
            else {
                Debug::Log("Tracker Thread Pointer is Null");
            }
        } else {
            Debug::Log("Tracker Object is Null");
        }
    }

    DLL_EXPORT void ObtainMap() {
        Debug::Log("Obtaining Map");
    }

    DLL_EXPORT void HookDeviceToZed() {
        Debug::Log("Hooking Device To Zed");
        if (to == nullptr) {
            Debug::Log("HookDeviceToZed: Tracker hasn't been initialized");
            return;
        }
        to->device = device;
    }

    DLL_EXPORT void SetTextureInitializedCallback(TrackerObject::FuncTextureInitializedCallback callback) {
        Debug::Log("Setting Texture Initialized Callback");
        if (to == nullptr) {
            Debug::Log("SetTextureInitializedCallback: Tracker hasn't been initialized");
            return;
        }
        to->texture_initialized_callback = callback;
    }

    DLL_EXPORT void RegisterMeshCompleteCallback(TrackerObject::FuncMeshCompleteCallback callback) { 
        Debug::Log("Registering Mesh Complete Callback");
        if (to == nullptr) {
            Debug::Log("RegisterMeshCompleteCallback: Tracker hasn't been initialized");
            return;
        }
        to->mesh_complete_callback = callback;
    }
    
    DLL_EXPORT void RegisterMeshCallback(TrackerObject::FuncMeshCallback my_callback) {
        Debug::Log("Registering Mesh Callback");
        if (to == nullptr) {
            Debug::Log("RegisterMeshCallback: Tracker hasn't been initialized");
            return;
        }
        to->mesh_callback = my_callback;
    }

    DLL_EXPORT void SetMapData(unsigned char* input_data, int Length) {
        Debug::Log("Setting Map Data");
    }

    DLL_EXPORT void StartSpatialMapping(int chunk_sizes) {
        Debug::Log("Starting Spatial Mapping");
        std::cout << "Arguments: " << chunk_sizes << std::endl;
        if (to == nullptr) {
            Debug::Log("StartSpatialMapping: Tracker hasn't been initialized");
            return;
        }
        to->start_spacial_mapping = true;
    }

    DLL_EXPORT void StopSpatialMapping(int chunk_sizes) {
        Debug::Log("Stopping Spatial Mapping");
        std::cout << "Arguments: " << chunk_sizes << std::endl;
        if (to == nullptr) {
            Debug::Log("StopSpatialMapping: Tracker hasn't been initialized");
            return;
        }
        to->stop_spacial_mapping = true;
    }

    DLL_EXPORT void SetRenderTexturePointer(void* texture_handle) {
        Debug::Log("Setting Render Texture Pointer");
        if (to == nullptr) {
            Debug::Log("SetRenderTexturePointer: Tracker hasn't been initialized");
            return;
        }
        to->d3d_texture = (ID3D11Texture2D*)texture_handle;
    }

    DLL_EXPORT void CompletedMeshUpdate() {
        Debug::Log("Completed Mesh Update");
        if (to == nullptr) {
            Debug::Log("CompletedMeshUpdate: Tracker hasn't been initialized");
            return;
        }
        to->request_new_chunks = true;
    }

    static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType) {
        Debug::Log("OnGraphicsDeviceEvent");
        if (eventType == kUnityGfxDeviceEventInitialize) {
            IUnityGraphicsD3D11* d3d = unity_interfaces->Get<IUnityGraphicsD3D11>();
            device = d3d->GetDevice();
        }
        else if (eventType == kUnityGfxDeviceEventShutdown) {
            Debug::Log("Shutdown");
        }       
    }

    extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload() {
        Debug::Log("Unity Plugin Unload");
        graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
    }

    extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces * unity_interfaces) {
        unity_interfaces = unity_interfaces;
        graphics = unity_interfaces->Get<IUnityGraphics>();
        graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent); 
        OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
    }

    DLL_EXPORT void RegisterDebugCallback(FuncCallBack cb);
    DLL_EXPORT void RegisterBinaryMapCallback(TrackerObject::FuncCallBack3 cb);
    DLL_EXPORT void RegisterObjectPoseCallback(TrackerObject::FuncCallBack4 cb);
    DLL_EXPORT void RegisterLocalizationCallback(TrackerObject::FuncCallBack2 cb);
}

static void UNITY_INTERFACE_API OnRenderEvent(int event_id) {
    Debug::Log("On Render Event");
    if (to != nullptr) {
        to->update_camera_texture_gpu();
    }
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
    Debug::Log("Tracker Thread Started");
    to->tracking(use_localization);
    delete to;
    to = nullptr;
    Debug::Log("Tracker Thread Finished");
}
