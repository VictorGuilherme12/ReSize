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
            wcout << L"Orientation: " << orientationToString(monitor.orientation) << endl;
        }

        // Find the 4K monitor (assuming it's the one with the highest resolution)
        auto it4K = max_element(monitors.begin(), monitors.end(),
            [](const MonitorInfo& a, const MonitorInfo& b) {
                return (a.width * a.height) < (b.width * b.height);
            });

        if (it4K != monitors.end()) {
            // Target resolutions
            const int TARGET_4K_WIDTH = 3840;
            const int TARGET_4K_HEIGHT = 2160;
            const int TARGET_FHD_WIDTH = 1920;
            const int TARGET_FHD_HEIGHT = 1080;

            // Current resolution check with more tolerance
            bool is4K = (it4K->width >= 3840 && it4K->width <= 3860) &&
                (it4K->height >= 2160 && it4K->height <= 2180);

            if (is4K) {
                // Switch to 1080p
                wcout << L"\nSwitching from 4K to 1080p..." << endl;
                if (ChangeResolution(it4K->deviceName, TARGET_FHD_WIDTH, TARGET_FHD_HEIGHT)) {
                    // Try setting another monitor as primary
                    for (const auto& monitor : monitors) {
                        if (monitor.deviceName != it4K->deviceName) {
                            if (SetPrimaryMonitor(monitor.deviceName)) {
                                break;
                            }
                        }
                    }
                }
            }
            else {
                // Switch to 4K
                wcout << L"\nSwitching to 4K..." << endl;
                if (ChangeResolution(it4K->deviceName, TARGET_4K_WIDTH, TARGET_4K_HEIGHT)) {
                    // Try setting another monitor as primary
                    for (const auto& monitor : monitors) {
                        if (monitor.deviceName != it4K->deviceName) {
                            if (SetPrimaryMonitor(monitor.deviceName)) {
                                break;
                            }
                        }
                    }
                }
            }

            // Get updated monitor information
            Sleep(1000); // Wait for changes to take effect
            auto updatedMonitors = EnumerateAllMonitors();
            wcout << L"\nUpdated monitor configuration:" << endl;
            for (const auto& monitor : updatedMonitors) {
                wcout << L"\nDevice: " << monitor.deviceName << endl;
                wcout << L"Resolution: " << monitor.width << L"x" << monitor.height << endl;
                wcout << L"Primary: " << (monitor.isPrimary ? L"Yes" : L"No") << endl;
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