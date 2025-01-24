#ifndef MONITORINFO_H
#define MONITORINFO_H

#include <string>
#include <windows.h>  // Para definir DWORD e outras fun��es da API do Windows
#include <vector>
#include <iostream>

struct MonitorInfo {
    std::wstring deviceName;
    int width;
    int height;
    bool isPrimary;
    DWORD stateFlags;  // Defini��o correta para DWORD
};

#endif // MONITORINFO_H
