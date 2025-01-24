#include "monitorutils.h"
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

    // Obter as configura��es de exibi��o atuais
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Falha ao obter as configura��es de exibi��o. Erro: " << GetLastError() << endl;
        return false;
    }

    // Preparar as novas configura��es de resolu��o
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    // Tentar mudar a resolu��o
    LONG result = ChangeDisplaySettingsEx(
        deviceName.c_str(),
        &dm,
        NULL,
        CDS_UPDATEREGISTRY | CDS_GLOBAL,
        NULL
    );

    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        wcout << L"Resolu��o alterada com sucesso." << endl;
        return true;
    case DISP_CHANGE_BADMODE:
        wcout << L"Modo gr�fico n�o suportado." << endl;
        break;
    case DISP_CHANGE_FAILED:
        wcout << L"Falha ao alterar as configura��es de exibi��o." << endl;
        break;
    case DISP_CHANGE_RESTART:
        wcout << L"Reinicializa��o necess�ria para aplicar as altera��es." << endl;
        break;
    default:
        wcout << L"Erro desconhecido ao mudar a resolu��o. C�digo: " << result << endl;
    }
    return false;
}

bool SetPrimaryMonitor(const wstring& deviceName) {
    DISPLAY_DEVICE dd = { sizeof(DISPLAY_DEVICE), {0} };
    DWORD deviceNum = 0;
    while (EnumDisplayDevices(NULL, deviceNum, &dd, 0)) {
        if (wcscmp(dd.DeviceName, deviceName.c_str()) == 0) {
            DEVMODE dm = { 0 };
            dm.dmSize = sizeof(DEVMODE);

            if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
                wcout << L"Falha ao obter configura��es de exibi��o. Erro: " << GetLastError() << endl;
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
                wcout << L"Monitor primario configurado com sucesso." << endl;
                return true;
            }
            else {
                wcout << L"Falha ao configurar o monitor prim�rio. C�digo de erro: " << result << endl;
                return false;
            }
        }
        deviceNum++;
    }

    wcout << L"Monitor n�o encontrado." << endl;
    return false;
}
