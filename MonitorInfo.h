#ifndef MONITORINFO_H
#define MONITORINFO_H

#include <string>
#include <windows.h>
#include <iostream>

using namespace std;

struct MonitorInfo {
    string deviceName;
    int width;
    int height;
    bool isPrimary;
    DWORD stateFlags;  // Definição correta para DWORD
};  // <-- Add semicolon here

#endif // MONITORINFO_H
