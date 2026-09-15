#pragma once

#ifndef IOMISCS_H_
#define IOMISCS_H_

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace IOMiscs
{
    /**
    * @brief Makes a path into cross platform.
    * 
    * @param inputPath String containing the path.
    * 
    * @return The path, with the preferred native separators.
    */
    fs::path MakeCrossPlatformPath(const std::string& inputPath);

    /**
    * @brief Makes a path into cross platform.
    *
    * @param inputPath String containing the path.
    *
    * @return The path, with the preferred native separators.
    */
    fs::path MakeCrossPlatformPath(const fs::path& inputPath);
}

#endif // IOMISCS_H_