#include "monitorutils.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    // Exemplo de enumerar monitores
    vector<MonitorInfo> monitors = EnumerateAllMonitors();
    for (const auto& monitor : monitors) {
        wcout << L"Device: " << wstring(monitor.deviceName.begin(), monitor.deviceName.end())
              << L", Resolucao: " << monitor.width << L"x" << monitor.height;
        if (monitor.isPrimary) {
            wcout << L" (Primario)" << endl;
        } else {
            wcout << L" (Secundario)" << endl;
        }
    }

    // Alterar a resolu��o do monitor 1 (DISPLAY1)
    const wstring targetDeviceName = L"\\\\.\\DISPLAY1";
    if (ChangeResolution(targetDeviceName, 1920, 1080)) {
        wcout << L"Resolucao do " << targetDeviceName << L" alterada para 1920x1080 com sucesso!" << endl;
    }

    // Definir o monitor 1 como prim�rio (DISPLAY1)
    if (SetPrimaryMonitor(targetDeviceName)) {
        wcout << L"Monitor " << targetDeviceName << L" configurado como primario." << endl;
    }

    return 0;
}
