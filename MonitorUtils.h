#ifndef MONITORUTILS_H
#define MONITORUTILS_H

#include "monitorinfo.h"
#include <string>
#include <vector>

using namespace std;

vector<MonitorInfo> EnumerateAllMonitors();
bool ChangeResolution(const wstring& deviceName, int width, int height);
bool SetPrimaryMonitor(const wstring& deviceName);

#endif // MONITORUTILS_H
