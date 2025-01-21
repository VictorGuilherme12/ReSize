#pragma once
#include <string>
#include <windows.h>

struct Position {
    int x;
    int y;
};

struct MonitorInfo {
    std::wstring deviceName;
    DWORD stateFlags;
    bool isPrimary;
    int width;
    int height;
    Position position;
    DWORD orientation;
};

inline std::wstring orientationToString(DWORD orientation) {
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