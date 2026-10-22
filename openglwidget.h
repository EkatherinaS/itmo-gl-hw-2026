#pragma once

#include <QElapsedTimer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QOpenGLWidget *parent = nullptr);

    virtual void onInit() = 0;
    virtual void onRender() = 0;
    virtual void onResize(size_t width, size_t height) = 0;

private:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

public:
signals:
    void fpsUpdated(double fps);

private:
    QElapsedTimer fpsTimer_;
    quint64 frameCount_ = 0;
    double fps_ = 0.0;
};
