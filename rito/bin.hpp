#ifndef RITO_BIN_HPP
#define RITO_BIN_HPP
#include "file.hpp"
#include "types.hpp"

namespace Rito {
    struct Bin {
        std::vector<std::string> linkedFiles;
        File::result_t read(File const& file) RITO_FILE_NOEXCEPT;
    };
}

#endif // RITO_BIN_HPP
