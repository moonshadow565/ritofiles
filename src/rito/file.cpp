#include <cstdio>
#include "file.hpp"

using namespace Rito;

const char *FileError::what() const noexcept {
    return std::runtime_error::what();
}

File::File(const char *name, bool write) {
    auto f = fopen(name, write ? "wb" : "rb");
    file_assert(f != nullptr);
    fseek(f, 0, SEEK_END);
    file = f;
    end = static_cast<int32_t>(ftell(f));
    fseek(f, 0, SEEK_SET);
}

File::~File() noexcept {
    if(file) {
        fclose(reinterpret_cast<FILE*>(file));
        file = nullptr;
        end = 0;
    }
}

int32_t File::tell() const {
    auto const result = static_cast<int32_t>(ftell(reinterpret_cast<FILE*>(file)));
    file_assert(result != -1);
    return result;
}

void File::seek_beg(int32_t offset) const {
    auto const result = fseek(reinterpret_cast<FILE*>(file), offset, SEEK_SET);
    file_assert(result == 0);
}

void File::seek_cur(int32_t offset) const {
    auto const result = fseek(reinterpret_cast<FILE*>(file), offset, SEEK_CUR);
    file_assert(result == 0);
}

void File::seek_end(int32_t offset) const {
    auto const result = fseek(reinterpret_cast<FILE*>(file), offset, SEEK_END);
    file_assert(result == 0);
}

void File::raw_read(void *data, int32_t size, int32_t count) const {
    auto const result = fread(data,
                              static_cast<size_t>(size),
                              static_cast<size_t>(count),
                              reinterpret_cast<FILE*>(file));
    file_assert(result == static_cast<size_t>(count));
}

