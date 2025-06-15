#define UNICODE
#define _UNICODE
#include "MonitorUtils.h"
#include <windows.h>

std::vector<MonitorInfo> EnumerateAllMonitors() {
    std::vector<MonitorInfo> monitors;
    DISPLAY_DEVICEW dd = { sizeof(DISPLAY_DEVICEW), {0} };
    DWORD deviceNum = 0;

    while (EnumDisplayDevicesW(nullptr, deviceNum, &dd, 0)) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            MonitorInfo info;
            info.deviceName = dd.DeviceName;
            info.stateFlags = dd.StateFlags;
            info.isPrimary = (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

            DEVMODEW dm = { 0 };
            dm.dmSize = sizeof(DEVMODEW);
            if (EnumDisplaySettingsW(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
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

MonitorStatus ChangeResolution(const std::wstring& deviceName, int width, int height) {
    DEVMODEW dm = { 0 };
    dm.dmSize = sizeof(DEVMODEW);

    if (!EnumDisplaySettingsW(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        return MonitorStatus::FAILED;
    }

    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG result = ChangeDisplaySettingsExW(
        deviceName.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY | CDS_GLOBAL, nullptr
    );

    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        return MonitorStatus::SUCCESS;
    case DISP_CHANGE_BADMODE:
        return MonitorStatus::BAD_MODE;
    case DISP_CHANGE_RESTART:
        return MonitorStatus::RESTART_REQUIRED;
    default:
        return MonitorStatus::FAILED;
    }
}

MonitorStatus SetPrimaryMonitor(const std::wstring& deviceName) {
    DEVMODEW dm = { 0 };
    dm.dmSize = sizeof(DEVMODEW);
    if (!EnumDisplaySettingsW(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        return MonitorStatus::MONITOR_NOT_FOUND;
    }

    dm.dmPosition.x = 0;
    dm.dmPosition.y = 0;
    dm.dmFields = DM_POSITION;

    LONG result = ChangeDisplaySettingsExW(
        deviceName.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY | CDS_SET_PRIMARY, nullptr
    );

    if (result == DISP_CHANGE_SUCCESSFUL) {
        // Reiniciar o Explorer para garantir que a barra de tarefas e ícones se atualizem corretamente
        system("taskkill /f /im explorer.exe > nul 2>&1");
        system("start explorer.exe");
        return MonitorStatus::SUCCESS;
    }

    return MonitorStatus::FAILED;
}