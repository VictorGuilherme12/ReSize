#include "MonitorUtils.h"
#include "ConsoleUI.h"
#include <iostream>
#include <vector>

int main() {
    ConsoleUI::setupConsole();

    while (true) {
        std::vector<MonitorInfo> monitors = EnumerateAllMonitors();
        ConsoleUI::printMonitorInfo(monitors);

        int option = ConsoleUI::displayMainMenu();

        if (option == 4) {
            break; // Sai do loop e encerra o programa
        }

        if (option < 1 || option > 3 || monitors.empty()) {
            std::wcout << L"\nOpção inválida ou nenhum monitor encontrado." << std::endl;
            ConsoleUI::waitAndClear();
            continue; // Volta para o início do loop
        }

        int monitorNum = ConsoleUI::selectMonitor(monitors);
        if (monitorNum == -1) {
            std::wcout << L"\nSeleção de monitor inválida!" << std::endl;
            ConsoleUI::waitAndClear();
            continue; // Volta para o início do loop
        }

        const std::wstring& targetDevice = monitors[monitorNum - 1].deviceName;
        MonitorStatus status;

        switch (option) {
        case 1: // 4K
            status = ChangeResolution(targetDevice, 3840, 2160);
            break;
        case 2: // Full HD
            status = ChangeResolution(targetDevice, 1920, 1080);
            break;
        case 3: // Alternar primário
            status = SetPrimaryMonitor(targetDevice);
            break;
        }

        ConsoleUI::showOperationResult(status);
        ConsoleUI::waitAndClear();
    }

    return 0;
}