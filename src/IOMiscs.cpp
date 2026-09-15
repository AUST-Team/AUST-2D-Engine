#include "IOMiscs.h"

fs::path IOMiscs::MakeCrossPlatformPath(const std::string& inputPath)
{
    std::string normalized = inputPath;
    // Convert Windows backslashes '\\' to generic slashes '/'.
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    // Constructing fs::path from normalized string automatically; converts '/' to native separators ('\\' on Windows, '/' on Linux).
    return fs::path(normalized).make_preferred();
}

fs::path IOMiscs::MakeCrossPlatformPath(const fs::path& inputPath)
{
    std::string normalized = inputPath.string();
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    return fs::path(normalized).make_preferred();
}
