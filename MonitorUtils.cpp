#include "MonitorUtils.h"
#include <windows.h>
#include <iostream>

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

bool ChangeResolution(const wstring& deviceName, int width, int height) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Failed to get current display settings. Error code: " << GetLastError() << endl;
        return false;
    }
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    // Primeiro tente sem CDS_UPDATEREGISTRY para uma mudança temporária
    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL,
        CDS_FULLSCREEN, NULL);

    if (result == DISP_CHANGE_SUCCESSFUL) {
        // Se for bem sucedido, aplique permanentemente
        result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL,
            CDS_UPDATEREGISTRY | CDS_FULLSCREEN, NULL);
    }

    return (result == DISP_CHANGE_SUCCESSFUL);
}

bool SetMonitorPosition(const wstring& deviceName, int x, int y, bool isPrimary) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Failed to get display settings. Error code: " << GetLastError() << endl;
        return false;
    }

    dm.dmFields = DM_POSITION;
    dm.dmPosition.x = x;
    dm.dmPosition.y = y;

    DWORD flags = CDS_UPDATEREGISTRY;
    if (isPrimary) {
        flags |= CDS_SET_PRIMARY;
    }

    // Primeiro tente sem CDS_UPDATEREGISTRY
    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL, flags & ~CDS_UPDATEREGISTRY, NULL);

    if (result == DISP_CHANGE_SUCCESSFUL) {
        // Se for bem sucedido, aplique permanentemente
        result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL, flags, NULL);

        // Força a atualização das configurações
        ChangeDisplaySettings(NULL, 0);
    }

    return (result == DISP_CHANGE_SUCCESSFUL);
}

bool SetPrimaryMonitor(const wstring& deviceName) {
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);

    // Obtém as configurações de exibição atuais
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Failed to get display settings. Error code: " << GetLastError() << endl;
        return false;
    }

    // Define os campos a serem modificados (sem necessidade de DM_POSITION se não for mover)
    dm.dmFields = DM_POSITION;

    // Aplica a mudança com a flag CDS_SET_PRIMARY
    LONG result = ChangeDisplaySettingsEx(
        deviceName.c_str(),
        &dm,
        NULL,
        CDS_SET_PRIMARY | CDS_UPDATEREGISTRY, 
        NULL
    );

    if (result != DISP_CHANGE_SUCCESSFUL) {
        wcout << L"Failed to set primary monitor. Error code: " << result << endl;
        return false;
    }

    // Sucesso!
    return true;
}

void TogglePrimaryMonitor() {
    // Obtenha todos os monitores
    vector<MonitorInfo> monitors = EnumerateAllMonitors();
    if (monitors.size() < 2) {
        wcout << L"Insufficient monitors to toggle." << endl;
        return;
    }

    // Alterne entre o primeiro e o segundo monitor
    for (size_t i = 0; i < monitors.size(); i++) {
        if (i == 0) {
            // Defina o primeiro monitor como primário
            wcout << L"Setting " << monitors[i].deviceName << L" as primary monitor." << endl;
            SetPrimaryMonitor(monitors[i].deviceName);
        }
        else if (i == 1) {
            // Defina o segundo monitor como primário
            wcout << L"Setting " << monitors[i].deviceName << L" as primary monitor." << endl;
            SetPrimaryMonitor(monitors[i].deviceName);
        }
    }
}