#ifndef RITO_FILE_HPP
#define RITO_FILE_HPP
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include <optional>
#include <exception>

#ifndef RITO_FILE_DISABLE_EXCEPT
#define file_assert(what) do { if(!(what)) { throw ::Rito::FileError(#what); } } while(false)
#define RITO_FILE_NOEXCEPT
#define RITO_FILE_RESULT_T uint32_t
#define RITO_FILE_RESULT_OK 0
#else
#define file_assert(what) do { if(!(what)) { return __LINE__; } } while(false)
#define RITO_FILE_NOEXCEPT noexcept
#define RITO_FILE_RESULT_T uint32_t
#define RITO_FILE_RESULT_OK 0
#endif

namespace Rito {
    inline constexpr struct zero_terminated_t {} zero_terminated = {};

    struct FileError : std::exception {
        using std::exception::exception;
        FileError() = default;
        FileError(FileError const&) = default;
        FileError(FileError&&) = default;
        FileError& operator=(FileError const&) = default;
        FileError& operator=(FileError&&) = default;
        FileError(std::string const& what) : FileError(what.c_str()) {}
    };

    struct File {
        FILE* file;
        size_t end;
        using result_t = RITO_FILE_RESULT_T;
        static inline constexpr result_t result_ok = RITO_FILE_RESULT_OK;

        inline File(FILE* file, size_t e = 0) noexcept : file(file), end(e) {
        }
        File(File && other) noexcept : file(other.file), end(other.end) {
            other.file = nullptr;
            other.end = 0;
        }
        inline ~File() noexcept {
            if(file) {
                fclose(file);
            }
        }
        File(File const&) = delete;
        void operator=(File const&) = delete;
        void operator=(File &&) = delete;

        static std::optional<File> readb(char const* name) noexcept {
            if(FILE* f = nullptr; fopen_s(&f, name, "rb") || !f) {
                return std::nullopt;
            } else {
                fseek(f, 0, SEEK_END);
                auto const e = static_cast<size_t>(ftell(f));
                fseek(f, 0, SEEK_SET);
                return File(f, e);
            }
        }
        static std::optional<File> writeb(char const* name) noexcept {
            if(FILE* f = nullptr; fopen_s(&f, name, "wb") || !f) {
                return std::nullopt;
            } else {
                return File(f);
            }
        }
        static std::optional<File> readtxt(char const* name) noexcept {
            if(FILE* f = nullptr; fopen_s(&f, name, "r") || !f) {
                return std::nullopt;
            } else {
                return File(f);
            }
        }
        static std::optional<File> writetxt(char const* name) noexcept {
            if(FILE* f = nullptr; fopen_s(&f, name, "w") || !f) {
                return std::nullopt;
            } else {
                return File(f);
            }
        }

        template<typename...Args>
        inline bool printf(char const* fmt, Args&&...args) const noexcept {
            return fprintf(file, fmt, std::forward<Args>(args)...) == sizeof...(Args);
        }

        template<typename...Args>
        inline bool scanf(char const* fmt, Args&&...args) const noexcept {
            return fscanf(file, fmt, std::forward<Args>(args)...) == sizeof...(Args);
        }

        inline int64_t tell() const noexcept {
            return ftell(file);
        }

        inline bool seek_beg(int64_t offset) const noexcept {
            return fseek(file, static_cast<long int>(offset), SEEK_SET) == 0;
        }

        inline bool seek_cur(int64_t offset) const noexcept {
            return fseek(file, static_cast<long int>(offset), SEEK_CUR) == 0;
        }

        inline bool seek_end(int64_t offset) const noexcept {
            return fseek(file, static_cast<long int>(offset), SEEK_END) == 0;
        }

        template<typename T>
        inline bool read(T* data, size_t count) const noexcept {
            return fread(data, sizeof(T), count, file) == count;
        }

        template<typename T>
        inline bool read(std::vector<T>& value, size_t count) const noexcept {
            auto const cur = static_cast<size_t>(tell());
            if((count * sizeof(T)) > (end - cur)) {
                return false;
            }
            value.resize(count);
            return read(&value[0], count);
        }

        inline bool read(std::string& value, size_t count) const noexcept {
            auto const cur = static_cast<size_t>(tell());
            if(count > (end - cur)) {
                return false;
            }
            value.resize(count);
            return read(&value[0], count);
        }

        inline bool read(std::string& value, zero_terminated_t) const noexcept {
            auto const cur = tell();
            size_t size = 0;
            for(int c;;size++) {
                c = fgetc(file);
                if(c == EOF) {
                    return false;
                }
                if(c == '\0') {
                    break;
                }
            }
            if(size == 0) {
                return true;
            }
            seek_beg(cur);
            value.resize(size);
            return read(value.data(), size + 1);
        }

        template<typename T>
        inline bool read(T& value) const noexcept {
            return read(&value, 1);
        }
    };
}
#endif // RITO_FILE_HPP
