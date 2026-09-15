#include "MemoryTracker.h"

#if defined(USE_VLD_MEMORY_TRACKER) && defined(_DEBUG)
#include <vld.h>
#endif // defined(USE_VLD_MEMORY_TRACKER) && defined(_DEBUG)

#if defined(USE_MEMORY_TRACKER) && defined(_DEBUG)

#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <functional>

#include <SDL3/SDL_log.h>

#include "ConstantConfiguration.h"

std::unordered_map<void*, MemoryTracker::AllocationInfo> MemoryTracker::allocationMap;    
size_t MemoryTracker::currentMemoryUsage = 0;
size_t MemoryTracker::peakMemoryUsage = 0;

void MemoryTracker::Init()
{
    std::atexit(ReportLeaks);
    std::at_quick_exit(ReportLeaks);
}

void MemoryTracker::ReportLeaks()
{
    std::filesystem::create_directories(ConstantConfiguration::defaultMemoryReportPath);
    std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(t);
    std::tm local_tm;
    localtime_s(&local_tm, &tt);
    std::stringstream ss;
    ss << std::put_time(&local_tm, "%Y%m%d_%H%M%S");
    std::string timestamp = ss.str();

    std::filesystem::path reportPath = std::filesystem::path(ConstantConfiguration::defaultMemoryReportPath) / ("MemoryReport_" + timestamp + ".log");

    std::ofstream reportFile(reportPath);
    if (!reportFile.is_open())
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "MemoryTracker.ReportLeaks: Failed to create leak report file at: %s", reportPath.string().c_str());
        return;
    }

    std::function<void(const std::string&)> logOutput = [&](const std::string& message) {
        std::cout << message;
        reportFile << message;
    };

    std::stringstream out;
    out << "\n--- MEMORY REPORT ---\n";
    out << "Peak Memory Usage: " << peakMemoryUsage << " bytes\n";

    if (allocationMap.empty())
    {
        out << "Great job! Zero leaks detected.\n";
    }
    else
    {
        out << "ALERT: Detected " << allocationMap.size() << " memory leak(s):\n";
        size_t totalLeaked = 0;

        for (const auto& [ptr, info] : allocationMap)
        {
            out << "  -> Leaked " << info.size << " bytes at address " << ptr
                << " | Allocated in " << info.file << " on line " << info.line << "\n";
            totalLeaked += info.size;
        }
        out << "Total Leaked Memory: " << totalLeaked << " bytes\n";
    }
    out << "---------------------\n\n";

    logOutput(out.str());
}

void* MemoryTracker::Allocate(size_t size, const char* file, int line)
{
    if (size == 0)
    {
        return nullptr;
    }

    void* ptr = std::malloc(size);

    if (ptr)
    {
        allocationMap[ptr] = AllocationInfo {
            .size = size, 
            .file = file, 
            .line = line 
        };

        currentMemoryUsage += size;
        if (currentMemoryUsage > peakMemoryUsage)
        {
            peakMemoryUsage = currentMemoryUsage;
        }
    }
    else
    {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "MemoryTracker.Allocate: Failed to allocate %zu bytes at %s:%d", size, file, line);
    }

    return ptr;
}

void MemoryTracker::Deallocate(void* ptr)
{
    if (!ptr)
    {
        return;
    }

    auto it = allocationMap.find(ptr);
    if (it != allocationMap.end())
    {
        currentMemoryUsage -= it->second.size;

        allocationMap.erase(it);
    }
    else
    {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "MemoryTracker.Deallocate: Attempted to deallocate untracked pointer at address %p", ptr);
    }

    std::free(ptr);
}

#endif // defined(USE_MEMORY_TRACKER) && defined(_DEBUG)

#if defined(USE_CRT_MEMORY_TRACKER) && defined(_DEBUG)

namespace CRTMemoryTracker
{
    void Init()
    {
        int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        flags |= _CRTDBG_ALLOC_MEM_DF;
        flags |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(flags);
    }
}

#endif // defined(USE_CRT_MEMORY_TRACKER) && defined(_DEBUG)

#if defined(USE_VLD_MEMORY_TRACKER) && defined(_DEBUG)

#include <chrono>
#include <filesystem>

#include "ConstantConfiguration.h"

namespace VLDMemoryTracker
{
    void Init()
    {
        std::filesystem::create_directories(ConstantConfiguration::defaultVLDMemoryReportPath);

        std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(t);
        std::tm local_tm;
        localtime_s(&local_tm, &tt);

        std::wostringstream woss;
        woss << L"MemoryLogsVLD/VLDReport_"
            << std::put_time(&local_tm, L"%Y%m%d_%H%M%S")
            << L".log";

        VLDSetReportOptions(VLD_OPT_REPORT_TO_FILE, woss.str().c_str());
        VLDEnable();
    }
}

#endif // defined(USE_VLD_MEMORY_TRACKER) && defined(_DEBUG)
