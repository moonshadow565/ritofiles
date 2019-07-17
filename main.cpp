#include <iostream>
#include "rito/file.hpp"
#include "rito/simpleskin.hpp"
#include "rito/skeleton.hpp"
#include "rito/animation.hpp"
#include "rito/blend.hpp"

using namespace std;

int main() {
    //auto file = Rito::File::readb("Anivia4/BlackIceAnivia.blnd");
    //auto file = Rito::File::readb("Anivia3/Animations/anivia_blackice_channel.anm"); // v4
    auto file = Rito::File::readb("Anivia4/Animations/anivia_blackice_channel.anm"); // v5
    //auto file = Rito::File::readb("Anivia4/Animations/anivia_blackice_revive.anm"); // comp
    //auto file = Rito::File::readb("Karthus/Skins/Base/Animations/karthus_attack1.anm"); // lgcy
    //auto file = Rito::File::readb("Anivia4/Anivia.skl");
    try {
        Rito::Animation test{};
        // Rito::Skeleton test{};
        test.read(*file);
        for(auto const& c : test.tracks[3].frames) {
            printf("%f %f %f %f\n", c.quat.x, c.quat.y, c.quat.z, c.quat.w);
        }
    } catch(std::exception const& err) {
        puts("Error: ");
        puts(err.what());
    }
    return 0;
}
