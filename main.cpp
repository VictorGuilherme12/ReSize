#include "monitorutils.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fcntl.h>  // para _O_U16TEXT
#include <io.h>     // para _setmode

using namespace std;

void printMonitorInfo(const vector<MonitorInfo>& monitors) {
    wcout << L"\nInformações dos Monitores:" << endl;
    for (const auto& monitor : monitors) {
        wcout << L"Device: " << monitor.deviceName
              << L", Resolucao: " << monitor.width << L"x" << monitor.height;
        if (monitor.isPrimary) {
            wcout << L" (Primario)" << endl;
        } else {
            wcout << L" (Secundario)" << endl;
        }
    }
    wcout << endl;
}

int main() {
    // Configurar console para Unicode
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    while (true) {
        // Listar monitores e suas configurações atuais
        vector<MonitorInfo> monitors = EnumerateAllMonitors();
        printMonitorInfo(monitors);

        // Menu de opções
        wcout << L"Escolha uma opção:" << endl;
        wcout << L"1. Mudar para 4K (3840x2160)" << endl;
        wcout << L"2. Mudar para Full HD (1920x1080)" << endl;
        wcout << L"3. Alternar monitor primário" << endl;
        wcout << L"4. Sair" << endl;
        wcout << L"Opção: ";

        int opcao;
        wcin >> opcao;

        if (opcao == 4) break;

        // Selecionar monitor
        wcout << L"\nEscolha o monitor (1 para DISPLAY1, 2 para DISPLAY2, etc): ";
        int monitorNum;
        wcin >> monitorNum;

        if (monitorNum <= 0 || monitorNum > monitors.size()) {
            wcout << L"Monitor inválido!" << endl;
            continue;
        }

        const wstring targetDevice = monitors[monitorNum - 1].deviceName;

        switch (opcao) {
            case 1: // 4K
                if (ChangeResolution(targetDevice, 3840, 2160)) {
                    wcout << L"Resolução alterada para 4K com sucesso!" << endl;
                }
                break;

            case 2: // Full HD
                if (ChangeResolution(targetDevice, 1920, 1080)) {
                    wcout << L"Resolução alterada para Full HD com sucesso!" << endl;
                }
                break;

            case 3: // Alternar primário
                if (SetPrimaryMonitor(targetDevice)) {
                    wcout << L"Monitor configurado como primário com sucesso!" << endl;
                }
                break;

            default:
                wcout << L"Opção inválida!" << endl;
        }

        // Pequena pausa para ver o resultado
        wcout << L"\nAguarde..." << endl;
        this_thread::sleep_for(chrono::seconds(2));
    }

    return 0;
}