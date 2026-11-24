#include "gltfloader.h"

#include <QFile>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#define TINYGLTF_COMPONENT_TYPE_BYTE (5120)
#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE (5121)
#define TINYGLTF_COMPONENT_TYPE_SHORT (5122)
#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT (5123)
#define TINYGLTF_COMPONENT_TYPE_INT (5124)
#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT (5125)
#define TINYGLTF_COMPONENT_TYPE_FLOAT (5126)
#define TINYGLTF_COMPONENT_TYPE_DOUBLE (5130)

GltfLoader::GltfLoader()
{
    loader_ = tinygltf::TinyGLTF();
}

int GltfLoader::loadModel(GltfModel& gltfmodel, const char* path)
{
    QFile modelFile(path);
    if (!modelFile.open(QIODevice::ReadOnly)) {
        std::cout << "model file does not exist" << std::endl;
        return -1;
    }

    QByteArray modelData = modelFile.readAll();
    modelFile.close();
    if (modelData.isEmpty()) {
        std::cout << "model file is empty" << std::endl;
        return -1;
    }

    std::string err, warn;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(modelData.data());
    unsigned int size = static_cast<unsigned int>(modelData.size());

    bool success = loader_.LoadBinaryFromMemory(&model_, &err, &warn, data, size);
    if (!success) {
        std::cout << warn << std::endl;
        std::cout << err << std::endl;
        return -1;
    }
    std::cout << "model file successfully loaded" << std::endl;

    loadBuffers();
    std::cout << "buffers loaded" << std::endl;

    loadScene(0);
    std::cout << "scene loaded" << std::endl;

    gltfmodel.nodes_ = std::move(nodes_);
    gltfmodel.indexData_ = std::move(indexData_);
    gltfmodel.attributeSettings_ = std::move(attributeSettings_);
    gltfmodel.vertexData_ = {};
    for (const auto& [key, value] : attributeData_) {
        gltfmodel.attributeSettings_[key].offset = gltfmodel.vertexData_.size();
        gltfmodel.vertexData_.insert(gltfmodel.vertexData_.end(), value.begin(), value.end());
    }

    std::cout << "model all set" << std::endl;

    return 0;
}

void GltfLoader::loadScene(const int& ind)
{
    tinygltf::Scene scene = model_.scenes[ind];

    QMatrix4x4 trs;
    trs.setToIdentity();

    for (int node_ind : scene.nodes) {
        loadNode(node_ind, trs);
    }
}

void GltfLoader::loadNode(const int& ind, QMatrix4x4 trs)
{
    tinygltf::Node node = model_.nodes[ind];

    if (node.matrix.size() == 16) {
        std::array<float, 16> floatData;
        std::transform(node.matrix.begin(), node.matrix.end(), floatData.begin(), [](double d) {
            return static_cast<float>(d);
        });
        QMatrix4x4 local(floatData.data());
        trs *= local.transposed();
    }

    if (node.mesh != -1) {
        loadMesh(node.mesh, trs);
    }

    for (int child_ind : node.children) {
        loadNode(child_ind, trs);
    }
}

void GltfLoader::loadMesh(const int& ind, QMatrix4x4 trs)
{
    tinygltf::Mesh mesh = model_.meshes[ind];
    for (const tinygltf::Primitive& primitive : mesh.primitives) {
        loadMeshPrimitive(primitive, trs);
    }
}

void GltfLoader::loadMeshPrimitive(const tinygltf::Primitive& primitive, QMatrix4x4 trs)
{
    tinygltf::Accessor accessor;
    std::vector<unsigned char> data;

    accessor = model_.accessors[primitive.indices];
    data = getBufferView(accessor);

    switch (accessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_SHORT:
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
        uint16_t* indices = reinterpret_cast<uint16_t*>(data.data());
        for (size_t i = 0; i < accessor.count; ++i) {
            indices[i] += vertexCount_;
        }
    }
    case TINYGLTF_COMPONENT_TYPE_INT:
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
        uint32_t* indices = reinterpret_cast<uint32_t*>(data.data());
        for (size_t i = 0; i < accessor.count; ++i) {
            indices[i] += vertexCount_;
        }
    }
    }

    GltfModel::Node node = {trs, accessor.count, 0, indexData_.size()};
    indexData_.insert(indexData_.end(), data.begin(), data.end());

    size_t vertexCount = 0;
    for (const auto& [name, ind] : primitive.attributes) {
        accessor = model_.accessors[ind];
        vertexCount = accessor.count;
        data = getBufferView(accessor);
        attributeData_[name].insert(attributeData_[name].end(), data.begin(), data.end());

        if (attributeSettings_.find(name) == attributeSettings_.end()) {
            size_t componentCount = tinygltf::GetNumComponentsInType(accessor.type);
            GLenum componentType = getComponentGlType(accessor.componentType);
            size_t componentSize = getComponentSize(accessor.componentType);

            attributeSettings_[name] = GltfModel::AttributeLayout(componentCount,
                                                                  componentType,
                                                                  componentSize * componentCount);
        }
    }
    vertexCount_ += vertexCount;
    node.vertexCount = vertexCount;
    nodes_.push_back(node);
}

std::vector<unsigned char> GltfLoader::getBufferView(const tinygltf::Accessor& accessor)
{
    tinygltf::Buffer& buffer = model_.buffers[0];
    size_t bufferOffset = 0;

    if (accessor.bufferView >= 0) {
        const tinygltf::BufferView& bufferView = model_.bufferViews[accessor.bufferView];
        buffer = model_.buffers[bufferView.buffer];
        bufferOffset = bufferView.byteOffset;
    }

    size_t componentSize = getComponentSize(accessor.componentType);
    size_t numComponents = tinygltf::GetNumComponentsInType(accessor.type);
    size_t dataSize = accessor.count * componentSize * numComponents;
    size_t startOffset = bufferOffset + accessor.byteOffset;

    std::vector<unsigned char> result(dataSize);
    std::memcpy(result.data(), buffer.data.data() + startOffset, dataSize);
    return result;
}

void GltfLoader::loadBuffers()
{
    for (const tinygltf::Buffer& buffer : model_.buffers) {
        buffers_.push_back(buffer.data);
    }
}

size_t GltfLoader::getComponentSize(int componentType)
{
    switch (componentType) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
        return sizeof(GLbyte);
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        return sizeof(GLubyte);
    case TINYGLTF_COMPONENT_TYPE_SHORT:
        return sizeof(GLshort);
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        return sizeof(GLushort);
    case TINYGLTF_COMPONENT_TYPE_INT:
        return sizeof(GLint);
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        return sizeof(GLuint);
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
        return sizeof(GLfloat);
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
        return sizeof(GLdouble);
    default:
        std::cout << "unknown type" << std::endl;
    }
    return 0;
}

GLenum GltfLoader::getComponentGlType(int componentType)
{
    switch (componentType) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
        return GL_BYTE;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        return GL_UNSIGNED_BYTE;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
        return GL_SHORT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        return GL_UNSIGNED_SHORT;
    case TINYGLTF_COMPONENT_TYPE_INT:
        return GL_INT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        return GL_UNSIGNED_INT;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
        return GL_FLOAT;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
        return GL_DOUBLE;
    default:
        std::cout << "unknown type" << std::endl;
    }
    return 0;
}
