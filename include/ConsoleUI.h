#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include "MonitorUtils.h" // Precisa de MonitorInfo e MonitorStatus
#include <string>
#include <vector>

namespace ConsoleUI {

    void setupConsole();
    void printMonitorInfo(const std::vector<MonitorInfo>& monitors);
    int displayMainMenu();
    int selectMonitor(const std::vector<MonitorInfo>& monitors);
    void showOperationResult(MonitorStatus status);
    void waitAndClear();

} // namespace ConsoleUI

#endif // CONSOLEUI_H