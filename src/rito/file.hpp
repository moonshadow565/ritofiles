#ifndef RITO_FILE_HPP
#define RITO_FILE_HPP
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <stdexcept>
#include <array>

#define file_assert(what) do { if(!(what)) { throw ::Rito::FileError(#what); } } while(false)

namespace Rito {
    template<size_t S>
    struct FixedString {
        char data[S];
        operator std::string_view() const & {
            auto const begin = std::begin(data);
            auto const end = std::end(data);
            auto const size = std::find(begin, end, '\0') - begin;
            return { begin, static_cast<size_t>(size) };
        }
        operator std::string() const {
            auto const begin = std::begin(data);
            auto const end = std::end(data);
            auto const size = std::find(begin, end, '\0') - begin;
            return { begin, static_cast<size_t>(size) };
        }
    };

    struct FileError : std::runtime_error {
        FileError(FileError const&) = default;
        FileError(FileError&&) = default;
        FileError& operator=(FileError const&) = default;
        FileError& operator=(FileError&&) = default;
        FileError(std::string const& what) noexcept : std::runtime_error::runtime_error(what) {}
        FileError(char const*  what) noexcept  : std::runtime_error::runtime_error(what) {}
        char const* what() const noexcept override;
    };

    struct zero_terminated_t {};
    inline constexpr zero_terminated_t zero_terminated = {};

    template<typename T>
    struct size_prefix_t {};
    template<typename T>
    inline constexpr size_prefix_t<T> size_prefix = {};

    template<int32_t S>
    struct size_fixed_t {};
    template<int32_t S>
    inline constexpr size_fixed_t<S> size_fixed = {};

    struct BaseResource {
        int32_t resourceSize;
    };

    template<typename T>
    struct WithOffset {
        int32_t offset;
        T value;

        inline T const* operator->() const noexcept {
            return &value;
        }

        inline T const& operator*() const noexcept {
            return value;
        }

        inline operator T const&() const noexcept {
            return value;
        }
    };

    template<typename T = void>
    struct Offset {
        int32_t offset;

        inline constexpr bool operator!() const noexcept {
            return offset == 0 || offset == -1;
        }

        inline constexpr explicit operator bool() const noexcept {
            return !!*this;
        }

        inline constexpr Offset<T> operator[](int32_t idx) const noexcept {
            return Offset<T> { offset + idx * static_cast<int32_t>(sizeof(T)) };
        }

        inline constexpr operator int32_t() const noexcept {
            return offset;
        }

        inline constexpr Offset<T> operator+(int32_t right) const noexcept {
            return Offset<T> { offset + right };
        }

        template<typename F, typename P>
        inline Offset<T> operator+(F P::* m) const noexcept {
            auto const o = reinterpret_cast<intptr_t>(&(reinterpret_cast<P const*>(0)->*m));
            return Offset<T> { offset + static_cast<int32_t>(o) };
        }

        template<typename W>
        inline constexpr Offset<T> operator+(WithOffset<W> const& right) const noexcept {
            return Offset<T> { offset + right.offset };
        }
    };

    template<typename T>
    struct FlexArr {
        inline constexpr Offset<T> operator[](int32_t idx) const noexcept {
            return Offset<T> { idx * sizeof(T) };
        }
    };

    template<typename T>
    using AbsPtr = Offset<T>;

    template<typename T>
    using RelPtr = Offset<T>;

    template<typename T>
    using AbsPtrArr = AbsPtr<AbsPtr<T>>;

    template<typename T>
    using RelPtrArr = RelPtr<RelPtr<T>>;

    struct File {
    private:
        void* file = {};
        int32_t end = {};
    public:
        File(char const* name, bool write = false);

        ~File() noexcept;

        inline File(File && other) noexcept {
            std::swap(file, other.file);
            std::swap(end, other.end);
        }

        File(File const&) = delete;

        void operator=(File const&) = delete;

        void operator=(File &&) = delete;

        int32_t tell() const;

        void seek_beg(int32_t offset) const;

        void seek_cur(int32_t offset) const;

        void seek_end(int32_t offset) const;

        void raw_read(void* data, int32_t size, int32_t count) const;

        template<typename T>
        inline void check_size(int32_t count) const {
            file_assert((count * static_cast<int32_t>(sizeof(T))) <= (end - tell()));
        }

        template<typename T, typename...ARGS>
        inline T get(ARGS&&...args) const {
            T value{};
            read(value, std::forward<ARGS>(args)...);
            return value;
        }

        template<typename T>
        inline void read(T& value) const {
            raw_read(&value, sizeof(T), 1);
        }

        template<typename T, typename...Args>
        inline void read(WithOffset<T>& value, Args&&...args) const {
            value.offset = { tell() };
            read(value.value, std::forward<Args>(args)...);
        }

        template<typename T, typename X, typename...Args>
        inline void read(T& value, Offset<X> offset, Args&&...args) const {
            auto const backup = tell();
            seek_beg(offset.offset);
            read(value, std::forward<Args>(args)...);
            seek_beg(backup);
        }

        template<typename T, typename X, typename...Args>
        inline void read(WithOffset<T>& value, Offset<X> offset, Args&&...args) const {
            auto const backup = tell();
            seek_beg(offset.offset);
            value.offset = { offset.offset };
            read(value.value, std::forward<Args>(args)...);
            seek_beg(backup);
        }

        template<typename T>
        inline void read(std::vector<T>& value, int32_t count) const {
            check_size<T>(count);
            value.resize(static_cast<size_t>(count));
            raw_read(value.data(), sizeof(T), count);
        }

        template<typename T, typename P>
        inline void read(std::vector<T>& value, size_prefix_t<P>) const {
            auto const size = get<P>();
            if(size > 0) {
                read(value, static_cast<int32_t>(size));
            }
        }

        inline void read(std::string& value, int32_t count) const {
            check_size<char>(count);
            value.resize(static_cast<size_t>(count));
            raw_read(value.data(), 1, count);
        }

        inline void read(std::string& value, zero_terminated_t) const {
            for(auto const start = tell();;) {
                char c;
                read(c);
                if(c == '\0') {
                    auto const end = tell();
                    auto const size = end - start;
                    seek_beg(start);
                    value.resize(static_cast<size_t>(size));
                    raw_read(value.data(), 1, size + 1);
                    break;
                }
            }
        }

        template<typename P>
        inline void read(std::string& value, size_prefix_t<P>) const {
            auto const size = get<P>();
            if(size > 0) {
                read(value, static_cast<int32_t>(size));
            }
        }

        template<size_t S>
        inline void read(std::string& value, size_fixed_t<S>) const {
            auto const data = get<std::array<char, S>>();
            data[S - 1] = '\0';
            value = data.data();
        }
    };
}
#endif // RITO_FILE_HPP
