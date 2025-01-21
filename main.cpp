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

        // Get all monitor information
        auto monitors = EnumerateAllMonitors();
        if (monitors.empty()) {
            std::wcout << L"No active display devices found!" << std::endl;
            system("pause");
            return 1;
        }

        // Print information about all monitors
        std::wcout << L"Found " << monitors.size() << L" display devices:" << std::endl;
        for (const auto& monitor : monitors) {
            std::wcout << L"\nDevice: " << monitor.deviceName << std::endl;
            std::wcout << L"Resolution: " << monitor.width << L"x" << monitor.height << std::endl;
            std::wcout << L"Primary: " << (monitor.isPrimary ? L"Yes" : L"No") << std::endl;
            std::wcout << L"Position: (" << monitor.position.x << L"," << monitor.position.y << L")" << std::endl;
            std::wcout << L"Orientation: " << orientationToString(monitor.orientation) << std::endl;
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
            std::wcout << L"Position: (" << it4K->position.x << L"," << it4K->position.y << L")" << std::endl;
            std::wcout << L"Orientation: " << orientationToString(it4K->orientation) << std::endl;

            // Determine current mode and switch
            if (it4K->width == 3840 && it4K->height == 2160) {
                // If currently in 4K, switch to 1080p and set as primary
                std::wcout << L"\nSwitching from 4K to 1080p..." << std::endl;
                if (!ChangeResolution(it4K->deviceName, 1920, 1080)) {
                    std::wcout << L"Failed to change resolution to 1080p!" << std::endl;
                }
                Sleep(1000);

                std::wcout << L"Setting 1080p monitor as primary..." << std::endl;
                if (!SetPrimaryMonitor(it4K->deviceName)) {
                    std::wcout << L"Failed to set monitor as primary!" << std::endl;
                }
            }
            else {
                // If currently in 1080p or other resolution, switch to 4K and set as secondary
                std::wcout << L"\nSwitching to 4K..." << std::endl;
                if (!ChangeResolution(it4K->deviceName, 3840, 2160)) {
                    std::wcout << L"Failed to change resolution to 4K!" << std::endl;
                }
                Sleep(1000);

                // Find and set another monitor as primary
                bool primarySet = true;
                for (const auto& monitor : monitors) {
                    if (monitor.deviceName != it4K->deviceName) {
                        std::wcout << L"Setting " << monitor.deviceName << L" as primary..." << std::endl;
                        if (SetPrimaryMonitor(monitor.deviceName)) {
                            primarySet = true;
                            break;
                        }
                        else {
                            std::wcout << L"Failed to set monitor as primary, trying next..." << std::endl;
                        }
                    }
                }

                Sleep(1000); // Wait for primary change to process

                // Now ensure the 4K monitor is secondary
                std::wcout << L"Ensuring 4K monitor is secondary..." << std::endl;
             
            }

            // Get updated monitor information
            Sleep(1000); // Wait for changes to take effect
            auto updatedMonitors = EnumerateAllMonitors();
            std::wcout << L"\nUpdated monitor configuration:" << std::endl;
            for (const auto& monitor : updatedMonitors) {
                std::wcout << L"\nDevice: " << monitor.deviceName << std::endl;
                std::wcout << L"Resolution: " << monitor.width << L"x" << monitor.height << std::endl;
                std::wcout << L"Primary: " << (monitor.isPrimary ? L"Yes" : L"No") << std::endl;
                std::wcout << L"Position: (" << monitor.position.x << L"," << monitor.position.y << L")" << std::endl;
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