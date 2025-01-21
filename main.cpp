#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include "MonitorInfo.h"
#include "MonitorUtils.h"

using namespace std;

int main() {
    try {
        // Set console to Unicode mode
        if (_setmode(_fileno(stdout), _O_U16TEXT) == -1 ||
            _setmode(_fileno(stderr), _O_U16TEXT) == -1) {
            throw runtime_error("Failed to set Unicode mode");
        }

        wcout << L"=== Monitor Configuration Diagnostic Tool ===\n" << endl;

        // Get all monitor information
        auto monitors = EnumerateAllMonitors();
        if (monitors.empty()) {
            wcout << L"No active display devices found!" << endl;
            system("pause");
            return 1;
        }

        // Print information about all monitors
        wcout << L"Found " << monitors.size() << L" display devices:" << endl;
        for (const auto& monitor : monitors) {
            wcout << L"\nDevice: " << monitor.deviceName << endl;
            wcout << L"Resolution: " << monitor.width << L"x" << monitor.height << endl;
            wcout << L"Primary: " << (monitor.isPrimary ? L"Yes" : L"No") << endl;
            wcout << L"Position: (" << monitor.position.x << L"," << monitor.position.y << L")" << endl;
            wcout << L"Orientation: " << orientationToString(monitor.orientation) << endl;
        }

        // Find the 4K monitor (assuming it's the one with the highest resolution)
        auto it4K = max_element(monitors.begin(), monitors.end(),
            [](const MonitorInfo& a, const MonitorInfo& b) {
                return (a.width * a.height) < (b.width * b.height);
            });

        if (it4K != monitors.end()) {
            wcout << L"\n=== Found potential 4K monitor ===" << endl;
            wcout << L"Device: " << it4K->deviceName << endl;
            wcout << L"Current resolution: " << it4K->width << L"x" << it4K->height << endl;
            wcout << L"Primary: " << (it4K->isPrimary ? L"Yes" : L"No") << endl;
            wcout << L"Position: (" << it4K->position.x << L"," << it4K->position.y << L")" << endl;
            wcout << L"Orientation: " << orientationToString(it4K->orientation) << endl;

            // Determine current mode and switch
            if (it4K->width == 3840 && it4K->height == 2160) {
                // If currently in 4K, switch to 1080p and set as primary
                wcout << L"\nSwitching from 4K to 1080p..." << endl;
                if (!ChangeResolution(it4K->deviceName, 1920, 1080)) {
                    wcout << L"Failed to change resolution to 1080p!" << endl;
                }
                Sleep(1000);

                wcout << L"Setting 1080p monitor as primary..." << endl;
                if (!SetPrimaryMonitor(it4K->deviceName)) {
                    wcout << L"Failed to set monitor as primary!" << endl;
                }
            }
            else {
                // If currently in 1080p or other resolution, switch to 4K and set as secondary
                wcout << L"\nSwitching to 4K..." << endl;
                if (!ChangeResolution(it4K->deviceName, 3840, 2160)) {
                    wcout << L"Failed to change resolution to 4K!" << endl;
                }
                Sleep(1000);

                // Find and set another monitor as primary
                bool primarySet = true;
                for (const auto& monitor : monitors) {
                    if (monitor.deviceName != it4K->deviceName) {
                        wcout << L"Setting " << monitor.deviceName << L" as primary..." << endl;
                        if (SetPrimaryMonitor(monitor.deviceName)) {
                            primarySet = true;
                            break;
                        }
                        else {
                            wcout << L"Failed to set monitor as primary, trying next..." << endl;
                        }
                    }
                }

                Sleep(1000); // Wait for primary change to process

                // Now ensure the 4K monitor is secondary
                wcout << L"Ensuring 4K monitor is secondary..." << endl;
             
            }

            // Get updated monitor information
            Sleep(1000); // Wait for changes to take effect
            auto updatedMonitors = EnumerateAllMonitors();
            wcout << L"\nUpdated monitor configuration:" << endl;
            for (const auto& monitor : updatedMonitors) {
                wcout << L"\nDevice: " << monitor.deviceName << endl;
                wcout << L"Resolution: " << monitor.width << L"x" << monitor.height << endl;
                wcout << L"Primary: " << (monitor.isPrimary ? L"Yes" : L"No") << endl;
                wcout << L"Position: (" << monitor.position.x << L"," << monitor.position.y << L")" << endl;
            }
        }
        else {
            wcout << L"No suitable monitor found!" << endl;
        }

        wcout << L"\nConfiguration attempt completed. Press any key to exit...\n";
        system("pause");
    }
    catch (const exception& e) {
        wcerr << L"Error: " << e.what() << endl;
        system("pause");
        return 1;
    }
    return 0;
}