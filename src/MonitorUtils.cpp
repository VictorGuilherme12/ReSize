#define UNICODE
#define _UNICODE
#include "include/MonitorUtils.h"
#include <windows.h>
#include <wingdi.h>
#include <iostream>
#include <vector>
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
            } else {
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
    // Primeiro, liberar as configurações atuais
    ChangeDisplaySettingsExW(nullptr, nullptr, nullptr, 0, nullptr);

    DEVMODEW dm = { 0 };
    dm.dmSize = sizeof(DEVMODEW);

    // Pegar as configurações atuais
    if (!EnumDisplaySettingsW(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        wcout << L"Falha ao obter as configurações de exibição. Erro: " << GetLastError() << endl;
        return false;
    }

    // Guardar configurações atuais
    int currentWidth = dm.dmPelsWidth;
    int currentHeight = dm.dmPelsHeight;
    int currentFrequency = dm.dmDisplayFrequency;
    int currentBitsPerPel = dm.dmBitsPerPel;

    // Se já estiver na resolução desejada, tentar uma resolução intermediária primeiro
    if (currentWidth == width && currentHeight == height) {
        // Usar uma resolução intermediária (por exemplo, 1280x720)
        dm.dmPelsWidth = 1280;
        dm.dmPelsHeight = 720;
        dm.dmDisplayFrequency = currentFrequency;
        dm.dmBitsPerPel = currentBitsPerPel;
        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

        LONG intermediateResult = ChangeDisplaySettingsExW(
            deviceName.c_str(),
            &dm,
            nullptr,
            CDS_UPDATEREGISTRY | CDS_GLOBAL,
            nullptr
        );

        // Pequena pausa para garantir que a mudança seja processada
        Sleep(1000);
    }

    // Configurar para a resolução desejada
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmDisplayFrequency = currentFrequency;
    dm.dmBitsPerPel = currentBitsPerPel;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

    LONG result = ChangeDisplaySettingsExW(
        deviceName.c_str(),
        &dm,
        nullptr,
        CDS_UPDATEREGISTRY | CDS_GLOBAL,
        nullptr
    );

    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        wcout << L"Resolução alterada com sucesso para " << width << L"x" << height
              << L" @ " << dm.dmDisplayFrequency << L"Hz" << endl;
        return true;

    case DISP_CHANGE_BADMODE:
        {
            DEVMODEW testDm = { 0 };
            testDm.dmSize = sizeof(DEVMODEW);
            int modeNum = 0;
            bool foundMode = false;

            while (EnumDisplaySettingsW(deviceName.c_str(), modeNum, &testDm)) {
                if (testDm.dmPelsWidth == width &&
                    testDm.dmPelsHeight == height) {

                    dm.dmDisplayFrequency = testDm.dmDisplayFrequency;
                    dm.dmBitsPerPel = testDm.dmBitsPerPel;

                    result = ChangeDisplaySettingsExW(
                        deviceName.c_str(),
                        &dm,
                        nullptr,
                        CDS_UPDATEREGISTRY | CDS_GLOBAL,
                        nullptr
                    );

                    if (result == DISP_CHANGE_SUCCESSFUL) {
                        wcout << L"Resolução alterada com sucesso para " << width << L"x" << height
                              << L" @ " << dm.dmDisplayFrequency << L"Hz" << endl;
                        return true;
                    }
                }
                modeNum++;
            }
            wcout << L"Não foi possível encontrar um modo compatível." << endl;
        }
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

    dm.dmPelsWidth = currentWidth;
    dm.dmPelsHeight = currentHeight;
    dm.dmDisplayFrequency = currentFrequency;
    dm.dmBitsPerPel = currentBitsPerPel;

    ChangeDisplaySettingsExW(
        deviceName.c_str(),
        &dm,
        nullptr,
        CDS_UPDATEREGISTRY | CDS_GLOBAL,
        nullptr
    );

    return false;
}

bool SetPrimaryMonitor(const wstring& deviceName) {
    DISPLAY_DEVICEW dd = { sizeof(DISPLAY_DEVICEW), {0} };
    DWORD deviceNum = 0;
    bool foundTarget = false;

    // Encontrar o monitor alvo
    while (EnumDisplayDevicesW(nullptr, deviceNum, &dd, 0)) {
        if (wcscmp(dd.DeviceName, deviceName.c_str()) == 0) {
            foundTarget = true;
            break;
        }
        deviceNum++;
    }

    if (!foundTarget) {
        wcout << L"Monitor não encontrado!" << endl;
        return false;
    }

    // Obter as configurações atuais do monitor alvo
    DEVMODEW dmNew = { 0 };
    dmNew.dmSize = sizeof(DEVMODEW);
    if (!EnumDisplaySettingsW(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dmNew)) {
        wcout << L"Falha ao obter configurações do monitor." << endl;
        return false;
    }

    // Manter a posição e definir como primário
    dmNew.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT;
    dmNew.dmPosition.x = 0;
    dmNew.dmPosition.y = 0;

    LONG res = ChangeDisplaySettingsExW(deviceName.c_str(), &dmNew, nullptr, CDS_UPDATEREGISTRY | CDS_GLOBAL, nullptr);
    if (res == DISP_CHANGE_SUCCESSFUL) {
        wcout << L"Monitor alterado para primário com sucesso!" << endl;

        // Reiniciar o Explorer para garantir que o Windows reconheça a mudança
        system("taskkill /f /im explorer.exe");
        system("start explorer.exe");

        return true;
    } else {
        wcout << L"Falha ao definir monitor primário. Código de erro: " << res << endl;
        return false;
    }
}
