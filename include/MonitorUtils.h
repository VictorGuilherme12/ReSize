#ifndef MONITORUTILS_H
#define MONITORUTILS_H

#include <string>
#include <vector>
#include <windows.h> // Para DWORD

// Estrutura para manter as informações de um monitor
struct MonitorInfo {
    std::wstring deviceName;
    int width;
    int height;
    bool isPrimary;
    DWORD stateFlags;
};

// Enum para status de retorno das operações
enum class MonitorStatus {
    SUCCESS,
    FAILED,
    BAD_MODE,
    RESTART_REQUIRED,
    MONITOR_NOT_FOUND
};

// Declaração das funções que serão implementadas em MonitorUtils.cpp
std::vector<MonitorInfo> EnumerateAllMonitors();
MonitorStatus ChangeResolution(const std::wstring& deviceName, int width, int height);
MonitorStatus SetPrimaryMonitor(const std::wstring& deviceName);

#endif // MONITORUTILS_H