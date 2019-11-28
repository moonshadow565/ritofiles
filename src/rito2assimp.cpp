#include "rito2assimp.hpp"
#include <filesystem>
#include <rito/simpleskin.hpp>
#include <rito/skeleton.hpp>

using namespace Rito;

extern std::unique_ptr<aiScene> Rito::ImportSkin(char const* skn_path, char const* skl_path) {
    auto scene = std::make_unique<aiScene>();
    auto r_skn = SimpleSkin { skn_path };
    auto r_skl = Skeleton { skl_path };

    if (r_skn.submeshes.size() == 0) {
        auto skn_name = std::filesystem::path(skn_path).filename().stem().string();
        auto skl_name = std::filesystem::path(skl_path).filename().stem().string();
        r_skn.submeshes.push_back({
                                      .name = skn_name,
                                      .firstVertex = 0,
                                      .vertexCount = static_cast<int32_t>(r_skn.vtxPositions.size()),
                                      .firstIndex = 0,
                                      .indexCount = static_cast<int32_t>(r_skn.indices.size())
                                  });

    }

    return scene;
}
