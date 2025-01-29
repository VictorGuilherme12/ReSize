#ifndef MONITORINFO_H
#define MONITORINFO_H

#include <string>
#include <windows.h>
#include <iostream>

using namespace std;

struct MonitorInfo {
    wstring deviceName;
    unsigned long stateFlags;
    bool isPrimary;
    int width;
    int height;
};

#endif // MONITORINFO_H
