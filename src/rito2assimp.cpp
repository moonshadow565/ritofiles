#include "rito2assimp.hpp"
#include <filesystem>
#include <iostream>
#include <rito/simpleskin.hpp>
#include <rito/skeleton.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// using namespace Assimp;
using namespace Rito;
std::unique_ptr<aiScene> Rito::ImportSkin(char const* skn_path, char const* skl_path) {
    auto scene = std::make_unique<aiScene>();
    auto r_skn = SimpleSkin { skn_path };
    auto r_skl = Skeleton { skl_path };

    auto skn_name = std::filesystem::path(skn_path).filename().stem().string();
    if (r_skn.submeshes.size() == 0) {
        r_skn.submeshes.push_back({
                                      .name = skn_name,
                                      .firstVertex = 0,
                                      .vertexCount = static_cast<int32_t>(r_skn.vtxPositions.size()),
                                      .firstIndex = 0,
                                      .indexCount = static_cast<int32_t>(r_skn.indices.size())
                                  });
    }

    auto rootNode = new aiNode();
    auto sknNode = new aiNode();
    sknNode->mName = skn_name;
    rootNode->addChildren(1, &sknNode);

    std::vector<aiNode*> boneNodes = {};
    boneNodes.reserve(r_skl.joints.size());
    for(auto const& joint: r_skl.joints) {
        auto bone = new aiNode();
        bone->mName = joint.name;
        boneNodes.push_back(bone);
    }
    for(size_t i = 0; i < r_skl.joints.size(); i++) {
        auto const& joint = r_skl.joints[i];
        auto& bone = boneNodes[i];
        if(joint.parentIndx != -1) {
            auto const& parent = boneNodes[(uint32_t)joint.parentIndx];
            parent->addChildren(1, &bone);

        } else {
            sknNode->addChildren(1, &bone);
        }
    }

    std::vector<aiMesh*> meshes = {};
    std::vector<aiMaterial*> materials = {};
    for (auto const& submesh : r_skn.submeshes) {
        auto meshNode = new aiNode();
        meshNode->mName = submesh.name;
        meshNode->mNumMeshes = 1;
        meshNode->mMeshes = new unsigned int[1] { (uint32_t)(meshes.size()) };
        sknNode->addChildren(1, &meshNode);

        auto mesh = meshes.emplace_back(new aiMesh());
        mesh->mMaterialIndex = (uint32_t)(materials.size());
        materials.emplace_back(new aiMaterial());
        mesh->mName = submesh.name;

        mesh->mNumFaces = (uint32_t)submesh.indexCount / 3;
        mesh->mFaces = new aiFace[mesh->mNumFaces];
        mesh->mNumVertices = (uint32_t)submesh.vertexCount;
        mesh->mVertices = new aiVector3D[mesh->mNumVertices];
        mesh->mNormals = new aiVector3D[mesh->mNumVertices];

        // TODO:
        // mTextureCoords, mNumUVComponents
        // mColors

        for (uint32_t f = (uint32_t)submesh.firstIndex, t = 0;
            f < (uint32_t)(submesh.firstIndex + submesh.indexCount);
            f += 3, t++) {
            mesh->mFaces[t].mNumIndices = 3;
            auto const a = r_skn.indices[f + 0] - (uint32_t)submesh.firstVertex;
            auto const b = r_skn.indices[f + 1] - (uint32_t)submesh.firstVertex;
            auto const c = r_skn.indices[f + 2] - (uint32_t)submesh.firstVertex;
            mesh->mFaces[t].mIndices = new unsigned int[3] { a, b, c };
        }

        std::vector<std::vector<std::pair<uint32_t, float>>> boneWeights;
        boneWeights.resize(r_skl.joints.size());
        for(uint32_t f = (uint32_t)submesh.firstVertex, t = 0;
            f < (uint32_t)(submesh.firstVertex + submesh.vertexCount);
            f ++, t++) {
            mesh->mVertices[t] = {
                r_skn.vtxPositions[f].x,
                r_skn.vtxPositions[f].y,
                r_skn.vtxPositions[f].z,
            };
            mesh->mNormals[t] = {
                r_skn.vtxNormals[f].x,
                r_skn.vtxNormals[f].y,
                r_skn.vtxNormals[f].z,
            };
            float sum = 0;
            for(size_t i = 0; i < 4; i++) {
                size_t boneIndex = r_skn.vtxBlendIndices[f][i];
                float weight = r_skn.vtxBlendWeights[f][i];
                // if ( weight > 0.0f) {
                // if (weight != 0.0f) {
                if ((weight - 0.00001f) > 0.0f) {
                    boneWeights[boneIndex].push_back({f - (uint32_t)submesh.firstVertex, weight});
                    sum += weight;
                }
            }
        }

        mesh->mBones = new aiBone*[r_skl.joints.size()];
        mesh->mNumBones = 0;
        for (size_t i = 0; i < r_skl.joints.size(); i++) {
            auto const& joint = r_skl.joints[i];
            auto const& weights = boneWeights[i];
            auto bone = new aiBone();
            bone->mName = joint.name;
            if(!weights.empty()) {
                bone->mNumWeights = (uint32_t)(weights.size());
                bone->mWeights = new aiVertexWeight[weights.size()];
                for(size_t c = 0; c < weights.size(); c++) {
                    bone->mWeights[c].mVertexId = weights[c].first;
                    bone->mWeights[c].mWeight = weights[c].second;
                }
            }
            memcpy(&bone->mOffsetMatrix, &joint.invRootOffset, sizeof(Mtx44));
            //bone->mArmature = skeleton;
            //bone->mNode = boneNodes[i];
            mesh->mBones[mesh->mNumBones] = bone;
            mesh->mNumBones++;
        }
    }
    scene->mRootNode = rootNode;

    scene->mNumMeshes = (uint32_t)(meshes.size());
    scene->mMeshes = new aiMesh*[meshes.size()];
    for(uint32_t i = 0; i < (uint32_t)(meshes.size()); i++) {
        scene->mMeshes[i] = meshes[i];
    }

    scene->mNumMaterials = (uint32_t)(materials.size());
    scene->mMaterials = new aiMaterial*[materials.size()];
    for(uint32_t i = 0; i < (uint32_t)(materials.size()); i++) {
        scene->mMaterials[i] = materials[i];
    }

    return scene;
}
