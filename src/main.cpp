#include <iostream>
#include <filesystem>
#include "rito2assimp.hpp"

using namespace std;

namespace fs = std::filesystem;

int main() {
    // Rito::ImportSkin("Ashe/Ashe.skn", "Ashe/Ashe.skl");
    Rito::ImportSkin("Pyke_Base.skn", "Ashe/Ashe.skl");

    return 0;
}
