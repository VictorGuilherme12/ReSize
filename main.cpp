#include <windows.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <io.h>
#include <vector>

struct MonitorInfo {
    std::wstring deviceName = L"";
    int width = 0;
    int height = 0;
    bool isPrimary = false;
    DWORD stateFlags = 0;
};

std::vector<MonitorInfo> EnumerateAllMonitors() {
    std::vector<MonitorInfo> monitors;
    DISPLAY_DEVICE dd = { sizeof(DISPLAY_DEVICE), {0} };
    DWORD deviceNum = 0;

    std::wcout << L"\n=== Enumerating All Display Devices ===\n" << std::endl;

    while (EnumDisplayDevices(NULL, deviceNum, &dd, 0)) {
        if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            MonitorInfo info;
            info.deviceName = dd.DeviceName;
            info.stateFlags = dd.StateFlags;
            info.isPrimary = (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

            DEVMODE dm = { 0 };
            dm.dmSize = sizeof(DEVMODE);

            if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
                info.width = dm.dmPelsWidth;
                info.height = dm.dmPelsHeight;
            }
            else {
                info.width = -1;
                info.height = -1;
            }

            std::wcout << L"Found Display Device:" << std::endl;
            std::wcout << L"  Device Name: " << info.deviceName << std::endl;
            std::wcout << L"  Device String: " << dd.DeviceString << std::endl;
            std::wcout << L"  State Flags: 0x" << std::hex << info.stateFlags << std::dec << std::endl;
            std::wcout << L"  Primary: " << (info.isPrimary ? L"Yes" : L"No") << std::endl;
            std::wcout << L"  Resolution: " << info.width << L"x" << info.height << std::endl;
            std::wcout << std::endl;

            monitors.push_back(info);
        }
        deviceNum++;
    }

    return monitors;
}

bool ChangeResolution(const std::wstring& deviceName, int width, int height) {
    std::wcout << L"\nAttempting to change resolution for " << deviceName << std::endl;
    std::wcout << L"Target resolution: " << width << L"x" << height << std::endl;

    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);

    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        std::wcout << L"Failed to get current display settings. Error code: " << GetLastError() << std::endl;
        return false;
    }

    std::wcout << L"Current resolution: " << dm.dmPelsWidth << L"x" << dm.dmPelsHeight << std::endl;

    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL,
        CDS_UPDATEREGISTRY | CDS_FULLSCREEN, NULL);

    std::wcout << L"ChangeDisplaySettingsEx result: ";
    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        std::wcout << L"SUCCESSFUL" << std::endl;
        return true;
    case DISP_CHANGE_BADFLAGS:
        std::wcout << L"BADFLAGS" << std::endl;
        break;
    case DISP_CHANGE_BADMODE:
        std::wcout << L"BADMODE" << std::endl;
        break;
    case DISP_CHANGE_BADPARAM:
        std::wcout << L"BADPARAM" << std::endl;
        break;
    case DISP_CHANGE_FAILED:
        std::wcout << L"FAILED" << std::endl;
        break;
    case DISP_CHANGE_RESTART:
        std::wcout << L"RESTART REQUIRED" << std::endl;
        break;
    default:
        std::wcout << L"UNKNOWN ERROR (" << result << L")" << std::endl;
    }
    return false;
}

bool SetPrimaryMonitor(const std::wstring& deviceName) {
    std::wcout << L"\nAttempting to set primary monitor: " << deviceName << std::endl;

    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);

    if (!EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
        std::wcout << L"Failed to get display settings. Error code: " << GetLastError() << std::endl;
        return false;
    }

    dm.dmFields = DM_POSITION;
    dm.dmPosition.x = 0;
    dm.dmPosition.y = 0;

    LONG result = ChangeDisplaySettingsEx(deviceName.c_str(), &dm, NULL,
        CDS_SET_PRIMARY | CDS_UPDATEREGISTRY, NULL);

    std::wcout << L"SetPrimaryMonitor result: ";
    switch (result) {
    case DISP_CHANGE_SUCCESSFUL:
        std::wcout << L"SUCCESSFUL" << std::endl;
        return true;
    case DISP_CHANGE_BADFLAGS:
        std::wcout << L"BADFLAGS" << std::endl;
        break;
    case DISP_CHANGE_BADMODE:
        std::wcout << L"BADMODE" << std::endl;
        break;
    case DISP_CHANGE_BADPARAM:
        std::wcout << L"BADPARAM" << std::endl;
        break;
    case DISP_CHANGE_FAILED:
        std::wcout << L"FAILED" << std::endl;
        break;
    case DISP_CHANGE_RESTART:
        std::wcout << L"RESTART REQUIRED" << std::endl;
        break;
    default:
        std::wcout << L"UNKNOWN ERROR (" << result << L")" << std::endl;
    }
    return false;
}

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

            // Determine current mode and switch
            if (it4K->width == 1920 && it4K->height == 1080 && it4K->isPrimary) {
                std::wcout << L"\nCurrently in gaming mode, switching to normal mode..." << std::endl;
                ChangeResolution(it4K->deviceName, 3840, 2160);
                Sleep(1000);

                // Find another monitor to set as primary
                for (const auto& monitor : monitors) {
                    if (monitor.deviceName != it4K->deviceName) {
                        SetPrimaryMonitor(monitor.deviceName);
                        break;
                    }
                }
            }
            else {
                std::wcout << L"\nCurrently in normal mode, switching to gaming mode..." << std::endl;
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