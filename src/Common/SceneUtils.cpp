#include "SceneUtils.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ranges>

namespace GPGVulkan
{

    void SaveSceneBinary(std::filesystem::path filepath, Scene *aScene)
    {
        std::ofstream outFile(filepath, std::ios::out | std::ios::binary);

        if (!outFile)
        {
            std::cerr << "An error occurred while opening the file for writing: " << filepath << std::endl;
            return;
        }

        aScene->serialize(outFile);

        if (!outFile)
        {
            std::cerr << "An error occurred while writing to the file: " << filepath << std::endl;
        }
        else
        {
            std::cout << "Data successfully saved to " << filepath << std::endl;
        }

        outFile.close();
    }
    Scene *LoadSceneBinary(std::filesystem::path aPath)
    {

        return nullptr;
    }

}