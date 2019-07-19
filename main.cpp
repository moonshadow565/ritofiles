#include <iostream>
#include <filesystem>
#include "rito/file.hpp"
#include "rito/simpleskin.hpp"
#include "rito/skeleton.hpp"
#include "rito/animation.hpp"
#include "rito/blend.hpp"

using namespace std;

namespace fs = std::filesystem;

int main() {
    //auto file = Rito::File::readb("Anivia4/BlackIceAnivia.blnd");
    //auto file = Rito::File::readb("Anivia3/Animations/anivia_blackice_channel.anm"); // v4
    //auto file = Rito::File::readb("Anivia4/Animations/anivia_blackice_channel.anm"); // v5
    //auto file = Rito::File::readb("Anivia4/Animations/anivia_blackice_revive.anm"); // comp
    //auto file = Rito::File::readb("Karthus/Skins/Base/Animations/karthus_attack1.anm"); // lgcy
    //auto file = Rito::File::readb("Anivia4/Anivia.skl");


    auto file = Rito::File::readb("blnd/BaseLiss.blnd");
    try {
        Rito::Blend test{};
        test.read(*file);
    } catch(std::exception const& err) {
        puts("Error: ");
        puts(err.what());
    }


    /*
    fs::path const root = "C:/Users/jesus/Desktop/420/DATA/Characters";
    for(auto const& entry : fs::recursive_directory_iterator(root)) {
        if(!entry.is_regular_file() || entry.is_directory()) {
            continue;
        }
        auto const path = entry.path();
        if(path.extension() != ".blnd") {
            continue;
        }
        try {
            if(auto file = Rito::File::readb(path.generic_string().c_str()); file) {
                Rito::Blend test{};
                test.read(*file);
            }
        } catch(std::exception const& err) {
            puts("Error: ");
            puts(err.what());
            puts("File: ");
            puts(path.generic_string().c_str());
        }
    }*/

    return 0;
}
