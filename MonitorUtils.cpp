#include "MonitorUtils.h"
#include <windows.h>
#include <iostream>
using namespace std;

vector<MonitorInfo> EnumerateAllMonitors() {
    vector<MonitorInfo> monitors;
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

bool ChangeResolution(const wstring& deviceName, int width, int height) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);

    // Get current display settings
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Failed to get current display settings. Error: " << GetLastError() << endl;
        return false;
    }

    // Prepare new display settings
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    // Attempt resolution change
    LONG result = ChangeDisplaySettingsEx(
        deviceName.c_str(),
        &dm,
        NULL,
        CDS_UPDATEREGISTRY | CDS_GLOBAL,
        NULL
    );

    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        wcout << L"Resolution changed successfully." << endl;
        return true;
    case DISP_CHANGE_BADMODE:
        wcout << L"Graphics mode not supported." << endl;
        break;
    case DISP_CHANGE_FAILED:
        wcout << L"Display settings change failed." << endl;
        break;
    case DISP_CHANGE_RESTART:
        wcout << L"Restart required to apply changes." << endl;
        break;
    default:
        wcout << L"Unknown error changing resolution. Code: " << result << endl;
    }
    return false;
}

bool SetPrimaryMonitor(const wstring& deviceName) {
    // Enumerate displays to find the device index
    DISPLAY_DEVICE dd = { sizeof(DISPLAY_DEVICE), {0} };
    DWORD deviceNum = 0;
    while (EnumDisplayDevices(NULL, deviceNum, &dd, 0)) {
        if (wcscmp(dd.DeviceName, deviceName.c_str()) == 0) {
            // Found the device, now set primary
            DEVMODE dm = { 0 };
            dm.dmSize = sizeof(DEVMODE);

            if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
                wcout << L"Failed to get display settings. Error: " << GetLastError() << endl;
                return false;
            }

            LONG result = ChangeDisplaySettingsEx(
                dd.DeviceName,
                &dm,
                NULL,
                CDS_UPDATEREGISTRY | CDS_SET_PRIMARY,
                NULL
            );

            if (result == DISP_CHANGE_SUCCESSFUL) {
                wcout << L"Primary monitor set successfully." << endl;
                return true;
            }
            else {
                wcout << L"Failed to set primary monitor. Error code: " << result << endl;
                return false;
            }
        }
        deviceNum++;
    }

    wcout << L"Monitor not found." << endl;
    return false;
}