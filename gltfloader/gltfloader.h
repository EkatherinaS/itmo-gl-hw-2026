#pragma once

#include "gltfloader_global.h"
#include "gltfmodel.h"
#include <tinygltf/tiny_gltf.h>

class GLTFLOADER_EXPORT GltfLoader
{
private:
    tinygltf::Model model_;
    tinygltf::TinyGLTF loader_;

    std::vector<std::vector<unsigned char>> buffers_;

    QMatrix4x4 trs_;
    std::vector<unsigned char> indexData_;
    std::vector<GltfModel::Node> nodes_;
    size_t vertexCount_ = 0;
    std::map<std::string, std::vector<unsigned char>> attributeData_;
    std::map<std::string, GltfModel::AttributeLayout> attributeSettings_;

    void loadScene(const int& ind);
    void loadNode(const int& ind, QMatrix4x4 trs);
    void loadMesh(const int& ind, QMatrix4x4 trs);
    void loadMeshPrimitive(const tinygltf::Primitive& primitive, QMatrix4x4 trs);
    void loadBuffers();

    GLenum getComponentGlType(int componentType);
    size_t getComponentSize(int componentType);
    std::vector<unsigned char> getBufferView(const tinygltf::Accessor& accessor);

public:
    GltfLoader();
    int loadModel(GltfModel& gltfmodel, const char* path);
};
