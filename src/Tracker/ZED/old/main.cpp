#include "common_header.h"
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <thread>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <array>
#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <math.h>
#include <float.h>
#include "Tracker_ZED.cpp" 

#define STB_IMAGE_IMPLEMENTATION
//-------------------------------------------------------------------
#define DLL_EXPORT __declspec(dllexport)

#ifdef __cplusplus  
extern "C" {


#endif
    TrackerObject* to;
    ID3D11Device* m_Device;

    std::thread* trackerThread;


    DLL_EXPORT void ObtainMap() { 
        if (to != nullptr) {
            to->GrabMap();
        }
    } 
    DLL_EXPORT void SetMapData(unsigned char* inputData, int Length) {
        if (to != nullptr) { 
            Debug::Log("Should Start Now");
            to->ImportMap(inputData, Length); 
        }
    } 
    
}