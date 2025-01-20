#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include "MonitorInfo.h"
#include "MonitorUtils.h"

int main() {
    try {
        // Set console to Unicode mode
        if (_setmode(_fileno(stdout), _O_U16TEXT) == -1 ||
            _setmode(_fileno(stderr), _O_U16TEXT) == -1) {
            throw std::runtime_error("Failed to set Unicode mode");
        }

        std::wcout << L"=== Monitor Configuration Diagnostic Tool ===\n" << std::endl;

        // Criando uma instância de MonitorInfo para exemplo
        MonitorInfo monitor;
        monitor.deviceName = L"Monitor 1";
        monitor.orientation = Orientation::Portrait; // Definindo a orientação para Portrait

        // Get all monitor information
        auto monitors = EnumerateAllMonitors();

        if (monitors.empty()) {
            std::wcout << L"No active display devices found!" << std::endl;
            system("pause");
            return 1;
        }

        // Find the 4K monitor (assuming it's the one with the highest resolution)
        auto it4K = std::max_element(monitors.begin(), monitors.end(),
            [](const MonitorInfo& a, const MonitorInfo& b) {
                return (a.width * a.height) < (b.width * b.height);
            });

        if (it4K != monitors.end()) {
            std::wcout << L"\n=== Found potential 4K monitor ===" << std::endl;
            std::wcout << L"Device: " << it4K->deviceName << std::endl;
            std::wcout << L"Current resolution: " << it4K->width << L"x" << it4K->height << std::endl;
            std::wcout << L"Primary: " << (it4K->isPrimary ? L"Yes" : L"No") << std::endl;
             std::wcout << L"Orientation: " << orientationToString(monitor.orientation) << std::endl;

            // Determine current mode and switch
            if (it4K->width == 1920 && it4K->height == 1080 && it4K->isPrimary) {
                ChangeResolution(it4K->deviceName, 3840, 2160);
                Sleep(1000);

                for (const auto& monitor : monitors) {
                    if (monitor.deviceName != it4K->deviceName) {
                        SetPrimaryMonitor(monitor.deviceName);
                        break;
                    }
                }
            }
            else {
                ChangeResolution(it4K->deviceName, 1920, 1080);
                Sleep(1000);
                SetPrimaryMonitor(it4K->deviceName);
            }
        }
        else {
            std::wcout << L"No suitable monitor found!" << std::endl;
        }

        std::wcout << L"\nConfiguration attempt completed. Press any key to exit...\n";
        system("pause");
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error: " << e.what() << std::endl;
        system("pause");
        return 1;
    }

    return 0;
}
