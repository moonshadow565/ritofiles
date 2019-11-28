#include "bin.hpp"

using namespace Rito;

void Rito::Bin::read(File const& file) {
    struct {
        std::array<char, 4> magic;
        uint32_t version;
    } header;

    file_assert(file.read(header));
    file_assert((header.magic == std::array{'P', 'R', 'O', 'P'}));
    if(header.version >= 2u) {
        uint32_t linkedFileCount = 0;
        file_assert(file.read(linkedFileCount));
        for(uint32_t i = 0; i < linkedFileCount; i++) {
            uint16_t linkedFileNameLength;
            file_assert(file.read(linkedFileNameLength));
            auto& linkedFileName = linkedFiles.emplace_back();
            linkedFileName.resize(linkedFileNameLength);
            file_assert(file.read(linkedFileName));
        }
    }
}
