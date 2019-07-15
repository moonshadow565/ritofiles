#include <iostream>
#include "rito/file.hpp"
#include "rito/simpleskin.hpp"
#include "rito/skeleton.hpp"
#include "rito/animation.hpp"

using namespace std;

int main() {
    auto file = Rito::File::readb("Karthus/Skins/Base/Animations/karthus_attack1.anm");
    try {
        Rito::Animation test{};
        test.read(*file);
    } catch(std::exception const& err) {
        puts("Error: ");
        puts(err.what());
    }
    return 0;
}
