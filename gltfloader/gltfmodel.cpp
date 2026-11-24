#include "gltfmodel.h"
#include <iostream>

void GltfModel::onInit(QOpenGLShaderProgram &program)
{
    initializeOpenGLFunctions();

    vao_.create();
    vao_.bind();

    vbo_.create();
    vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_.bind();

    ibo_.create();
    ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_.bind();

    program.bind();

    ibo_.allocate(indexData_.data(), static_cast<int>(indexData_.size()));
    vbo_.allocate(vertexData_.data(), static_cast<int>(vertexData_.size()));

    int attributeIndex = 0;
    for (const auto &[key, value] : attributeSettings_) {
        std::cout << "Attribute " << key << std::endl;
        std::cout << "  offset: " << value.offset << std::endl;
        std::cout << "  stride: " << value.stride << std::endl;
        std::cout << "  componentCount: " << value.componentCount << std::endl;
        std::cout << "  componentType: " << value.componentType << std::endl;

        program.enableAttributeArray(attributeIndex);
        program.setAttributeBuffer(attributeIndex,
                                   value.componentType,
                                   value.offset,
                                   value.componentCount,
                                   value.stride);

        ++attributeIndex;
    }

    trs_.setToIdentity();
    u_trs_ = program.uniformLocation("trs");

    program.release();

    vao_.release();
    vbo_.release();
    ibo_.release();
}

void GltfModel::onRender(QOpenGLShaderProgram &program)
{
    program.bind();
    vao_.bind();

    for (auto node : nodes_)
    {
        program.setUniformValue(u_trs_, node.trs);
        void *offset = reinterpret_cast<void *>(node.indexOffset);
        glDrawElements(GL_TRIANGLES, node.indexCount, GL_UNSIGNED_INT, offset);
    }

    vao_.release();
    program.release();
}

void GltfModel::calculateBoundingBox()
{
    AttributeLayout layout = attributeSettings_["POSITION"];
    unsigned char *buffer = vertexData_.data();
    boundingBox_.setDefault();

    unsigned char *ind = buffer + layout.offset;
    for (const Node &node : nodes_) {
        for (int i = 0; i < node.vertexCount; ++i) {
            float *x = reinterpret_cast<float *>(ind);
            float *y = reinterpret_cast<float *>(ind + sizeof(float));
            float *z = reinterpret_cast<float *>(ind + 2 * sizeof(float));
            ind += layout.stride;

            QVector3D pos = node.trs * QVector3D(*x, *y, *z);

            boundingBox_.xMin = std::min(pos.x(), boundingBox_.xMin);
            boundingBox_.xMax = std::max(pos.x(), boundingBox_.xMax);
            boundingBox_.yMin = std::min(pos.y(), boundingBox_.yMin);
            boundingBox_.yMax = std::max(pos.y(), boundingBox_.yMax);
            boundingBox_.zMin = std::min(pos.z(), boundingBox_.zMin);
            boundingBox_.zMax = std::max(pos.z(), boundingBox_.zMax);
        }
    }
}

void GltfModel::fitToBoundingBox(GLfloat maxWidth, GLfloat maxHeigth, GLfloat maxDepth)
{
    GltfModel::calculateBoundingBox();

    float width = boundingBox_.xMax - boundingBox_.xMin;
    float height = boundingBox_.yMax - boundingBox_.yMin;
    float depth = boundingBox_.zMax - boundingBox_.zMin;

    float scaleX = maxWidth / width;
    float scaleY = maxHeigth / height;
    float scaleZ = maxDepth / depth;

    float scale = std::min(scaleX, std::min(scaleY, scaleZ));

    QMatrix4x4 s;
    s.scale(scale);

    for (Node &node : nodes_) {
        node.trs = s * node.trs;
    }
}

void GltfModel::translateToCoordinates(GLfloat x, GLfloat y, GLfloat z)
{
    GltfModel::calculateBoundingBox();

    float width = boundingBox_.xMax - boundingBox_.xMin;
    float height = boundingBox_.yMax - boundingBox_.yMin;
    float depth = boundingBox_.zMax - boundingBox_.zMin;

    float translateX = width / 2 - boundingBox_.xMax + x;
    float translateY = height / 2 - boundingBox_.yMax + y;
    float translateZ = depth / 2 - boundingBox_.zMax + z;

    QMatrix4x4 t;
    t.translate(translateX, translateY, translateZ);

    for (Node &node : nodes_) {
        node.trs = t * node.trs;
    }
}
