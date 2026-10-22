#include "openglwidget.h"

#include <iostream>

OpenGLWidget::OpenGLWidget(QOpenGLWidget *parent)
    : QOpenGLWidget(parent)
{}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    onInit();

    fpsTimer_.start();
}

void OpenGLWidget::resizeGL(const int width, const int height)
{
    // Qt5
    // const auto dpr = devicePixelRatio();
    // onResize(static_cast<size_t>(width * dpr), static_cast<size_t>((height ? height : 1) * dpr));

    onResize(static_cast<size_t>(width), static_cast<size_t>(height ? height : 1));
}

void OpenGLWidget::paintGL()
{
    onRender();

    ++frameCount_;

    if (fpsTimer_.elapsed() >= 1000) {
        const double elapsedSeconds = fpsTimer_.restart() / 1000.0;
        fps_ = frameCount_ / elapsedSeconds;
        frameCount_ = 0;
        emit fpsUpdated(fps_);
    }
}
