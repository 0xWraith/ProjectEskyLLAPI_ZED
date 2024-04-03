
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
#include "common_header.h"
#include <sl/Camera.hpp>
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"
#include "IUnityInterface.h"
#include "IUnityGraphics.h"

class TrackerObject {
public:
    typedef void(*FuncTextureInitializedCallback) (int TextureWidth, int TextureHeight,int textureCount,float v_fov);
    FuncTextureInitializedCallback textureInitializedCallback = nullptr;
    
    typedef void(*FuncMeshCompleteCallback)();
    FuncMeshCompleteCallback meshCompleteCallback = nullptr;

    typedef void(*FuncMeshCallback)(int ChunkID, float* vertices, int verticesLength, float* normals, int normalsLength, float* uvs, int uvsLength, int* triangleIndicies, int triangleIndiciesLength);
    FuncMeshCallback meshCallback = nullptr;

    typedef void(*FuncCallBack2)(int LocalizationDelegate); 
    FuncCallBack2 callbackLocalization = nullptr;

    typedef void(*FuncCallBack3)(unsigned char* binaryData, int Length);
    FuncCallBack3 callbackBinaryMap = nullptr;

    typedef void(*FuncCallBack4)(string ObjectID, float tx, float ty, float tz, float qx, float qy, float qz, float qw);
    FuncCallBack4 callbackObjectPoseReceived = nullptr;
};