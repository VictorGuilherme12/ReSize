#pragma once
#include <string>
#include <vector>
#include "MonitorInfo.h"

vector<MonitorInfo> EnumerateAllMonitors();
bool ChangeResolution(const wstring& deviceName, int width, int height);
bool SetPrimaryMonitor(const wstring& deviceName);
