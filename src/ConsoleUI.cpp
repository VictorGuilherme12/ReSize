#define NOMINMAX

#include "ConsoleUI.h"
#include <iostream>
#include <limits>   // <--- CORREÇÃO PRINCIPAL: Incluído para std::numeric_limits
#include <fcntl.h>  // Para _O_U16TEXT
#include <io.h>     // Para _setmode
#include <stdlib.h> // Para system("cls")

using namespace std;

void ConsoleUI::setupConsole() {
    // Configura o console para aceitar caracteres Unicode (acentos, emojis)
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
}

void ConsoleUI::printMonitorInfo(const vector<MonitorInfo>& monitors) {
    wcout << L"🖥️  Informações dos Monitores Atuais:" << endl;
    for (size_t i = 0; i < monitors.size(); ++i) {
        wcout << L"  " << i + 1 << L". Device: " << monitors[i].deviceName
            << L", Resolução: " << monitors[i].width << L"x" << monitors[i].height;
        if (monitors[i].isPrimary) {
            wcout << L" (⭐ Primário)";
        }
        wcout << endl;
    }
    wcout << endl;
}

int ConsoleUI::displayMainMenu() {
    wcout << L"Escolha uma opção:" << endl;
    wcout << L"1. Mudar para 4K (3840x2160)" << endl;
    wcout << L"2. Mudar para Full HD (1920x1080)" << endl;
    wcout << L"3. Definir como monitor primário" << endl;
    wcout << L"4. Sair" << endl;
    wcout << L"Opção: ";

    int option;
    wcin >> option;
    return option;
}

int ConsoleUI::selectMonitor(const vector<MonitorInfo>& monitors) {
    wcout << L"\nEscolha o monitor para aplicar a alteração (1-" << monitors.size() << L"): ";
    int monitorNum;
    wcin >> monitorNum;

    if (wcin.fail() || monitorNum <= 0 || monitorNum > monitors.size()) {
        wcin.clear(); // Limpa o estado de erro
        wcin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta entrada inválida
        return -1; // Indica seleção inválida
    }
    return monitorNum;
}

void ConsoleUI::showOperationResult(MonitorStatus status) {
    switch (status) {
    case MonitorStatus::SUCCESS:
        wcout << L"\n✅ Operação realizada com sucesso!" << endl;
        break;
    case MonitorStatus::FAILED:
        wcout << L"\n❌ Falha ao executar a operação." << endl;
        break;
    case MonitorStatus::BAD_MODE:
        wcout << L"\n⚠️ O modo de vídeo não é suportado por este monitor." << endl;
        break;
    case MonitorStatus::RESTART_REQUIRED:
        wcout << L"\n🔄 É necessário reiniciar o computador para aplicar as alterações." << endl;
        break;
    case MonitorStatus::MONITOR_NOT_FOUND:
        wcout << L"\n🔍 Monitor não encontrado." << endl;
        break;
    }
}

void ConsoleUI::waitAndClear() {
    wcout << L"\nPressione ENTER para continuar..." << endl;

    // Se a última operação foi uma leitura de número (wcin >> ...),
    // o caractere '\n' (Enter) ainda está no buffer.
    // O primeiro ignore() limpa esse e quaisquer outros caracteres restantes.
    wcin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Agora o programa espera por uma nova entrada, que será apenas o usuário pressionando Enter.
    wcin.get();

    // Limpa a tela do console
    system("cls");
}