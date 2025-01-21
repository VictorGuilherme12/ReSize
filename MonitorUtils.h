#pragma once
#include <string>
#include <vector>
#include "MonitorInfo.h"

std::vector<MonitorInfo> EnumerateAllMonitors();
bool ChangeResolution(const std::wstring& deviceName, int width, int height);
bool SetPrimaryMonitor(const std::wstring& deviceName);
bool SetMonitorPosition(const std::wstring& deviceName, int x, int y, bool isPrimary);
bool SetSecondaryMonitor(const std::wstring& deviceName);