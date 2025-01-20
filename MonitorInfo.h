#ifndef MONITORINFO_H
#define MONITORINFO_H

#include <string>
#include <Windows.h>

struct MonitorInfo {
    std::wstring deviceName = L"";
    int width = 0;
    int height = 0;
    bool isPrimary = false;
    DWORD stateFlags = 0;
};

#endif // MONITORINFO_H