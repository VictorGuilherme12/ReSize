#ifndef MONITORUTILS_H
#define MONITORUTILS_H

#include "monitorinfo.h"
#include <string>
#include <vector>

using namespace std;


std::vector<MonitorInfo> EnumerateAllMonitors();
bool ChangeResolution(const std::wstring& deviceName, int width, int height);
bool SetPrimaryMonitor(const std::wstring& deviceName);

#endif // MONITORUTILS_H
