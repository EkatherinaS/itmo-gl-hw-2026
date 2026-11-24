#include "window.h"

#include <QLabel>
#include <iostream>

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

    // models/christmas_village.glb
    // models/coliseum.glb
    // models/portal_gun.glb

    gltfmodel_ = std::make_unique<GltfModel>();
    GltfLoader loader = GltfLoader();
    int result = loader.loadModel(*gltfmodel_, ":/models/portal_gun.glb");
    if (result != 0) {
        qDebug() << "Ошибка загрузки модели";
    }

    gltfmodel_->translateToCoordinates(0.0, 0.0, 0.0);
    gltfmodel_->fitToBoundingBox(1.0, 1.0, 1.0);
    gltfmodel_->onInit(*program_);

    // Configure uniforms
    modelUniform_ = program_->uniformLocation("model");
    viewUniform_ = program_->uniformLocation("view");
    projectionUniform_ = program_->uniformLocation("projection");

    viewPosUniform_ = program_->uniformLocation("view_pos");
    lightPosUniform_ = program_->uniformLocation("light_pos");
    lightColorUniform_ = program_->uniformLocation("light_color");

    program_->setUniformValue(lightColorUniform_, QVector3D(1.0f, 1.0f, 0.8f));

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

    model_.setToIdentity();
    model_.translate(0, 0, -1.5);
    model_.rotate(90, 1, 0, 0);

    view_.setToIdentity();

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

    program_->release();

    gltfmodel_->onRender(*program_);

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
