#define UNICODE
#define _UNICODE
#include "monitorutils.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

vector<MonitorInfo> EnumerateAllMonitors() {
    vector<MonitorInfo> monitors;
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

bool ChangeResolution(const wstring& deviceName, int width, int height) {
    DEVMODEW dm = { 0 };
    dm.dmSize = sizeof(DEVMODEW);

    if (!EnumDisplaySettingsW(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Falha ao obter as configurações de exibição. Erro: " << GetLastError() << endl;
        return false;
    }

    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG result = ChangeDisplaySettingsExW(
        deviceName.c_str(),
        &dm,
        nullptr,
        CDS_UPDATEREGISTRY | CDS_GLOBAL,
        nullptr
    );

    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        wcout << L"Resolução alterada com sucesso." << endl;
        return true;
    case DISP_CHANGE_BADMODE:
        wcout << L"Modo gráfico não suportado." << endl;
        break;
    case DISP_CHANGE_FAILED:
        wcout << L"Falha ao alterar as configurações de exibição." << endl;
        break;
    case DISP_CHANGE_RESTART:
        wcout << L"Reinicialização necessária para aplicar as alterações." << endl;
        break;
    default:
        wcout << L"Erro desconhecido ao mudar a resolução. Código: " << result << endl;
    }
    return false;
}

bool SetPrimaryMonitor(const wstring& deviceName) {
    DISPLAY_DEVICEW dd = { sizeof(DISPLAY_DEVICEW), {0} };
    DWORD deviceNum = 0;

    while (EnumDisplayDevicesW(nullptr, deviceNum, &dd, 0)) {
        if (wcscmp(dd.DeviceName, deviceName.c_str()) == 0) {
            DEVMODEW dm = { 0 };
            dm.dmSize = sizeof(DEVMODEW);

            if (!EnumDisplaySettingsW(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
                wcout << L"Falha ao obter configurações de exibição. Erro: " << GetLastError() << endl;
                return false;
            }

            LONG result = ChangeDisplaySettingsExW(
                dd.DeviceName,
                &dm,
                nullptr,
                CDS_UPDATEREGISTRY | CDS_SET_PRIMARY,
                nullptr
            );

            if (result == DISP_CHANGE_SUCCESSFUL) {
                wcout << L"Monitor primário configurado com sucesso." << endl;
                return true;
            }
            else {
                wcout << L"Falha ao configurar o monitor primário. Código de erro: " << result << endl;
                return false;
            }
        }
        deviceNum++;
    }

    wcout << L"Monitor não encontrado." << endl;
    return false;
}