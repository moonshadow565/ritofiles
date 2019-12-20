#include <iostream>
#include <filesystem>
#include "rito2assimp.hpp"
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include "rito/skeleton.hpp"
#include "rito/animation.hpp"

using namespace std;

namespace fs = std::filesystem;

int main() {
    auto scene = Rito::ImportSkin("Ashe/Ashe.skn", "Ashe/Ashe.skl");
    //auto scene = Rito::ImportSkin("Pyke/Pyke_Base.skn", "Pyke/Pyke_Base.skl");

    Assimp::Exporter exporter{};
    //exporter.Export(scene.get(), "objnomtl", "yolo.obj");
    unsigned int flags = 0;
    // flags |= aiProcess_ConvertToLeftHanded;
    flags |= aiProcess_PopulateArmatureData;
    exporter.Export(scene.get(), "collada", "yolo.dae", flags);
    return 0;
}
