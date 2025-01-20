#ifndef MONITORINFO_H
#define MONITORINFO_H

#include <string>
#include <Windows.h>

enum class Orientation {
    Landscape = 0,
    Portrait = 90,
    InvertedLandscape = 180, 
    InvertedPortrait = 270,
};

struct MonitorInfo {
    std::wstring deviceName = L"";
    int width = 0;
    int height = 0;
    bool isPrimary = false;
    Orientation orientation = Orientation::Landscape;
    DWORD stateFlags = 0;
};

inline std::wstring orientationToString(Orientation orientation) {
    switch (orientation) {
    case Orientation::Landscape:
        return L"Landscape";
    case Orientation::Portrait:
        return L"Portrait";
    case Orientation::InvertedLandscape:
        return L"Inverted Landscape";
    case Orientation::InvertedPortrait:
        return L"Inverted Portrait";
    default:
        return L"Unknown";
    }
}

#endif // MONITORINFO_H