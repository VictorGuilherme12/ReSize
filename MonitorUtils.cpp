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

    // Obter as configurações de exibição atuais
    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Falha ao obter as configurações de exibição. Erro: " << GetLastError() << endl;
        return false;
    }

    // Preparar as novas configurações de resolução
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    // Tentar mudar a resolução
    LONG result = ChangeDisplaySettingsEx(
        deviceName.c_str(),
        &dm,
        NULL,
        CDS_UPDATEREGISTRY | CDS_GLOBAL,
        NULL
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
    DISPLAY_DEVICE dd = { sizeof(DISPLAY_DEVICE), {0} };
    DWORD deviceNum = 0;
    while (EnumDisplayDevices(NULL, deviceNum, &dd, 0)) {
        if (wcscmp(dd.DeviceName, deviceName.c_str()) == 0) {
            DEVMODE dm = { 0 };
            dm.dmSize = sizeof(DEVMODE);

            if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
                wcout << L"Falha ao obter configurações de exibição. Erro: " << GetLastError() << endl;
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
                wcout << L"Falha ao configurar o monitor primário. Código de erro: " << result << endl;
                return false;
            }
        }
        deviceNum++;
    }

    wcout << L"Monitor não encontrado." << endl;
    return false;
}
