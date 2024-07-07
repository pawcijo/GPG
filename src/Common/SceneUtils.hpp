#pragma once

#include <filesystem>

#include "Scene.hpp"

namespace GPGVulkan
{

    void SaveSceneBinary(std::filesystem::path aPath);
    Scene *LoadSceneBinary(std::filesystem::path aPath);

}