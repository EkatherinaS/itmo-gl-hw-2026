#pragma once

#include "openglwidget.h"

#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QWindow>

class Window : public OpenGLWidget
{
    Q_OBJECT

public:
    Window();
    ~Window();

    void onInit() override;
    void onRender() override;
    void onResize(size_t width, size_t height) override;

private:
    GLint viewPosUniform_ = -1;
    GLint lightPosUniform_ = -1;
    GLint lightColorUniform_ = -1;

    GLint modelUniform_ = -1;
    GLint viewUniform_ = -1;
    GLint projectionUniform_ = -1;

    QElapsedTimer timer_;

    QMatrix4x4 model_;
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLVertexArrayObject vao_;

    std::unique_ptr<QOpenGLShaderProgram> program_;

    bool animated_ = true;
};
