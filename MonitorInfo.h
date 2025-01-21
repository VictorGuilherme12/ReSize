#pragma once
#include <string>
#include <windows.h>
using namespace std;

struct MonitorInfo {
    wstring deviceName = L"Unknown";
    DWORD stateFlags = 0;
    bool isPrimary = false;
    int width = -1;    
    int height = -1;    
    DWORD orientation = 0;
};

inline wstring orientationToString(DWORD orientation) {
    switch (orientation) {
    case DMDO_DEFAULT:
        return L"Landscape";
    case DMDO_90:
        return L"Portrait";
    case DMDO_180:
        return L"Landscape (flipped)";
    case DMDO_270:
        return L"Portrait (flipped)";
    default:
        return L"Unknown";
    }
}