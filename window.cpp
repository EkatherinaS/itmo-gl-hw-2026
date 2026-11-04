#include "window.h"

#include <QLabel>
#include <iostream>

// PURPLE CUBE ✩₊˚.⋆☾⋆⁺₊✧

constexpr std::array<GLfloat, 216u> vertices = {
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.965f, 0.820f, 0.984f, // 1
    0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.965f, 0.820f, 0.984f, // 2
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.965f, 0.820f, 0.984f, // 3
    -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.965f, 0.820f, 0.984f, // 4

    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.376f, 0.106f, 0.624f, // 5
    0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.376f, 0.106f, 0.624f, // 6
    0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.376f, 0.106f, 0.624f, // 7
    -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.376f, 0.106f, 0.624f, // 8

    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.231f, 0.035f, 0.329f, // 9
    -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.231f, 0.035f, 0.329f, // 10
    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  0.231f, 0.035f, 0.329f, // 11
    -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  0.231f, 0.035f, 0.329f, // 12

    0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.235f, 0.290f, 0.580f, // 13
    0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.235f, 0.290f, 0.580f, // 14
    0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  0.235f, 0.290f, 0.580f, // 15
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.235f, 0.290f, 0.580f, // 16

    -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.510f, 0.290f, 0.596f, // 17
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.510f, 0.290f, 0.596f, // 18
    0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.510f, 0.290f, 0.596f, // 19
    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.510f, 0.290f, 0.596f, // 20

    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.965f, 0.820f, 0.984f, // 21
    0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.965f, 0.820f, 0.984f, // 22
    0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.965f, 0.820f, 0.984f, // 23
    -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.965f, 0.820f, 0.984f, // 24
};

constexpr std::array<GLuint, 36u> indices = {0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,
                                             8,  9,  10, 8,  10, 11, 12, 13, 14, 12, 14, 15,
                                             16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

Window::Window()
{
    // Add label for FPS
    auto* fpsLabel = new QLabel("FPS: 0", this);
    fpsLabel->setStyleSheet("QLabel { color: white; }");
    fpsLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    fpsLabel->adjustSize();
    fpsLabel->move(0, 0);
    fpsLabel->raise();

    connect(this, &Window::fpsUpdated, [fpsLabel](double fps) {
        fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 0));
        fpsLabel->adjustSize();
    });
}

Window::~Window()
{
    program_.reset();
}

void Window::onInit()
{
    // Configure shaders
    program_ = std::make_unique<QOpenGLShaderProgram>(this);
    program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/template.vs");
    program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/template.fs");
    if (!program_->link()) {
        qDebug() << "Ошибка линковки:" << program_->log();
    }
    program_->bind();

    // Configure uniforms
    modelUniform_ = program_->uniformLocation("model");
    viewUniform_ = program_->uniformLocation("view");
    projectionUniform_ = program_->uniformLocation("projection");

    viewPosUniform_ = program_->uniformLocation("view_pos");
    lightPosUniform_ = program_->uniformLocation("light_pos");
    lightColorUniform_ = program_->uniformLocation("light_color");

    program_->setUniformValue(lightColorUniform_, QVector3D(1.0f, 1.0f, 0.8f));

    // Create VAO object
    vao_.create();
    vao_.bind();

    // Create VBO
    vbo_.create();
    vbo_.bind();
    vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

    // Create IBO
    ibo_.create();
    ibo_.bind();
    ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

    // Bind attributes
    program_->enableAttributeArray(0);
    program_->setAttributeBuffer(0, GL_FLOAT, 0, 3, static_cast<int>(9 * sizeof(GLfloat)));

    program_->enableAttributeArray(1);
    program_->setAttributeBuffer(1,
                                 GL_FLOAT,
                                 static_cast<int>(3 * sizeof(GLfloat)),
                                 3,
                                 static_cast<int>(9 * sizeof(GLfloat)));

    program_->enableAttributeArray(2);
    program_->setAttributeBuffer(2,
                                 GL_FLOAT,
                                 static_cast<int>(6 * sizeof(GLfloat)),
                                 3,
                                 static_cast<int>(9 * sizeof(GLfloat)));

    // Release all
    program_->release();

    vao_.release();
    vbo_.release();
    ibo_.release();

    // Start timer for animation (if needed)
    timer_.start();

    // Еnable depth test and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Clear all FBO buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate MVP matrix
    model_.setToIdentity();
    model_.translate(0.0, 0.0, -3.0);

    view_.setToIdentity();
    view_.translate(-1.0, -1.0, 0.0);
    view_.rotate(25.0, 1.0, -1.0, 0.0);

    // Bind shader program
    program_->bind();

    // Update uniform values
    program_->setUniformValue(modelUniform_, model_);
    program_->setUniformValue(viewUniform_, view_);
    program_->setUniformValue(projectionUniform_, projection_);

    program_->setUniformValue(viewPosUniform_, view_.inverted().column(3).toVector3D());

    if (animated_) {
        const float absoluteTime = timer_.elapsed() / 1000.0f;
        float lightXPos_ = std::sin(absoluteTime * 2.0f);
        program_->setUniformValue(lightPosUniform_, QVector3D(lightXPos_, -0.5f, 0.0f));
    }

    // Bind VAO
    vao_.bind();

    // Draw
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // Release all
    vao_.release();
    program_->release();

    if (animated_)
        update();
}

void Window::onResize(const size_t width, const size_t height)
{
    // Configure viewport
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    // Configure matrix
    const auto aspect = static_cast<float>(width) / static_cast<float>(height);
    const auto zNear = 0.1f;
    const auto zFar = 100.0f;
    const auto fov = 60.0f;
    projection_.setToIdentity();
    projection_.perspective(fov, aspect, zNear, zFar);
}
