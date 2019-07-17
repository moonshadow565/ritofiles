#include <iostream>
#include "rito/file.hpp"
#include "rito/simpleskin.hpp"
#include "rito/skeleton.hpp"
#include "rito/animation.hpp"
#include "rito/blend.hpp"

using namespace std;

int main() {
    auto file = Rito::File::readb("Anivia4/BlackIceAnivia.blnd");
    //auto file = Rito::File::readb("Anivia3/Animations/anivia_blackice_channel.anm");
    //auto file = Rito::File::readb("Anivia4/Animations/anivia_blackice_channel.anm");
    // auto file = Rito::File::readb("Anivia4/Animations/anivia_blackice_revive.anm");
    //auto file = Rito::File::readb("Karthus/Skins/Base/Animations/karthus_attack1.anm");
    //auto file = Rito::File::readb("Anivia3/Anivia.skl");
    try {
        Rito::Blend test{};
        // Rito::Skeleton test{};
        test.read(*file);
        /*
        for(auto const& c : test.tracks[0].frames) {
            printf("%f %f %f %f\n", c.quat.x, c.quat.y, c.quat.z, c.quat.w);
        }
        */
    } catch(std::exception const& err) {
        puts("Error: ");
        puts(err.what());
    }
    return 0;
}
