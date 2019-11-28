#ifndef RITO2ASSIMP_HPP
#define RITO2ASSIMP_HPP

#include <assimp/scene.h>
#include <memory>

namespace Rito {
    extern std::unique_ptr<aiScene> ImportSkin(char const* skn, char const* skl);
}


#endif // RITO2ASSIMP_HPP
