#include "MonitorUtils.h"
#include <windows.h>
#include <iostream>

std::vector<MonitorInfo> EnumerateAllMonitors() {
    std::vector<MonitorInfo> monitors;
    DISPLAY_DEVICE dd = { sizeof(DISPLAY_DEVICE), {0} };
    DWORD deviceNum = 0;

    while (EnumDisplayDevices(NULL, deviceNum, &dd, 0)) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            MonitorInfo info;
            info.deviceName = dd.DeviceName;
            info.stateFlags = dd.StateFlags;
            info.isPrimary = (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

            DEVMODE dm = { 0 };
            dm.dmSize = sizeof(DEVMODE);

            if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
                info.width = dm.dmPelsWidth;
                info.height = dm.dmPelsHeight;
            }
            else {
                info.width = -1;
                info.height = -1;
            }

            monitors.push_back(info);
        }
        deviceNum++;
    }

    return monitors;
}

bool ChangeResolution(const std::wstring& deviceName, int width, int height) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);

    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        std::wcout << L"Failed to get current display settings. Error code: " << GetLastError() << std::endl;
        return false;
    }

    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL,
        CDS_UPDATEREGISTRY | CDS_FULLSCREEN, NULL);

    return (result == DISP_CHANGE_SUCCESSFUL);
}

bool SetPrimaryMonitor(const std::wstring& deviceName) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);

    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        std::wcout << L"Failed to get display settings. Error code: " << GetLastError() << std::endl;
        return false;
    }

    dm.dmFields = DM_POSITION;
    dm.dmPosition.x = 0;
    dm.dmPosition.y = 0;

    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL,
        CDS_SET_PRIMARY | CDS_UPDATEREGISTRY, NULL);

    return (result == DISP_CHANGE_SUCCESSFUL);
}
