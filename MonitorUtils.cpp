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
                info.position.x = dm.dmPosition.x;
                info.position.y = dm.dmPosition.y;
            }
            else {
                info.width = -1;
                info.height = -1;
                info.position.x = 0;
                info.position.y = 0;
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

bool SetMonitorPosition(const std::wstring& deviceName, int x, int y, bool isPrimary) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        std::wcout << L"Failed to get display settings. Error code: " << GetLastError() << std::endl;
        return false;
    }

    dm.dmFields = DM_POSITION;
    dm.dmPosition.x = x;
    dm.dmPosition.y = y;

    DWORD flags = CDS_UPDATEREGISTRY;
    if (isPrimary) {
        flags |= CDS_SET_PRIMARY;
    }

    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL, flags, NULL);
    return (result == DISP_CHANGE_SUCCESSFUL);
}

bool SetPrimaryMonitor(const std::wstring& deviceName) {
    auto monitors = EnumerateAllMonitors();

    // Primeiro, encontre o monitor atual que é primário
    MonitorInfo* currentPrimary = nullptr;
    MonitorInfo* targetMonitor = nullptr;

    for (auto& monitor : monitors) {
        if (monitor.isPrimary) {
            currentPrimary = &monitor;
        }
        if (monitor.deviceName == deviceName) {
            targetMonitor = &monitor;
        }
    }

    if (!targetMonitor) {
        std::wcout << L"Target monitor not found!" << std::endl;
        return false;
    }

    // Define o novo monitor primário na posição (0,0)
    if (!SetMonitorPosition(deviceName, 0, 0, true)) {
        std::wcout << L"Failed to set primary monitor!" << std::endl;
        return false;
    }

    // Se havia um monitor primário anterior, mova-o para a direita
    if (currentPrimary && currentPrimary->deviceName != deviceName) {
        if (!SetMonitorPosition(currentPrimary->deviceName, targetMonitor->width, 0, false)) {
            std::wcout << L"Failed to reposition previous primary monitor!" << std::endl;
            return false;
        }
    }

    return true;
}

bool SetSecondaryMonitor(const std::wstring& deviceName) {
    auto monitors = EnumerateAllMonitors();
    MonitorInfo* primaryMonitor = nullptr;
    MonitorInfo* targetMonitor = nullptr;

    // Encontra o monitor primário e o monitor alvo
    for (auto& monitor : monitors) {
        if (monitor.isPrimary) {
            primaryMonitor = &monitor;
        }
        if (monitor.deviceName == deviceName) {
            targetMonitor = &monitor;
        }
    }

    if (!primaryMonitor || !targetMonitor) {
        std::wcout << L"Required monitors not found!" << std::endl;
        return false;
    }

    // Posiciona o monitor secundário à direita do primário
    return SetMonitorPosition(deviceName, primaryMonitor->width, 0, false);
}