#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QWindow>
#include "gltfloader_global.h"
#include <limits>

class GLTFLOADER_EXPORT GltfModel : protected QOpenGLFunctions
{
private:
    struct Node
    {
        size_t indexCount = 0;
        size_t vertexCount = 0;
        QMatrix4x4 trs;
        size_t indexOffset;

        Node() = default;
        Node(QMatrix4x4 matrix, size_t countInd, size_t countVert, size_t offset)
            : trs(matrix)
            , indexCount(countInd)
            , vertexCount(countVert)
            , indexOffset(offset)
        {}
    };

    struct AttributeLayout
    {
        int offset;
        int stride;
        int componentCount;
        GLenum componentType;

        AttributeLayout() = default;
        AttributeLayout(int componentCountValue,
                        GLenum componentTypeValue,
                        int strideValue,
                        int offsetValue = 0)
            : offset(offsetValue)
            , stride(strideValue)
            , componentType(componentTypeValue)
            , componentCount(componentCountValue)
        {}
    };

    struct BoundingBox
    {
        float xMin, xMax;
        float yMin, yMax;
        float zMin, zMax;

        void setDefault()
        {
            xMin = std::numeric_limits<float>::max();
            xMax = -std::numeric_limits<float>::max();
            yMin = std::numeric_limits<float>::max();
            yMax = -std::numeric_limits<float>::max();
            zMin = std::numeric_limits<float>::max();
            zMax = -std::numeric_limits<float>::max();
        }
    };

    std::vector<unsigned char> indexData_;
    std::vector<unsigned char> vertexData_;
    std::vector<Node> nodes_;
    std::map<std::string, AttributeLayout> attributeSettings_;

    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLVertexArrayObject vao_;

    GLint u_trs_ = -1;

    QMatrix4x4 trs_;
    BoundingBox boundingBox_;

    void calculateBoundingBox();

    friend class GltfLoader;

public:
    void onInit(QOpenGLShaderProgram &program);
    void onRender(QOpenGLShaderProgram &program);
    void translateToCoordinates(GLfloat x = 0.0, GLfloat y = 0.0, GLfloat z = 0.0);
    void fitToBoundingBox(GLfloat maxWidth = 1.0, GLfloat maxHeigth = 1.0, GLfloat maxDepth = 1.0);
};
