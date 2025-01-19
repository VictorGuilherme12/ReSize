#ifndef MONITORUTILS_H
#define MONITORUTILS_H

#include <vector>
#include <string>
#include "MonitorInfo.h"

// Enumerates all monitors connected to the system
std::vector<MonitorInfo> EnumerateAllMonitors();

// Changes the resolution of a specified monitor
bool ChangeResolution(const std::wstring& deviceName, int width, int height);

// Sets a specified monitor as the primary monitor
bool SetPrimaryMonitor(const std::wstring& deviceName);

#endif // MONITORUTILS_H
