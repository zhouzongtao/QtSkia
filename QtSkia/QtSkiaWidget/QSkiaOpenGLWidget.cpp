#include "QSkiaOpenGLWidget.h"

#include "core/SkImageInfo.h"
#include "core/SkSurface.h"
#include "core/SkCanvas.h"
#include "gpu/ganesh/SkSurfaceGanesh.h"
// Skia API changes: GrContext moved to ganesh
#include "gpu/ganesh/GrDirectContext.h"
#include "gpu/ganesh/gl/GrGLDirectContext.h"
#include "gpu/ganesh/gl/GrGLInterface.h"

#include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
class QSkiaOpenGLWidgetPrivate {
public:
    QOpenGLFunctions funcs;
    sk_sp<const GrGLInterface> glInterface = nullptr;
    sk_sp<GrDirectContext> context = nullptr;
    sk_sp<SkSurface> gpuSurface = nullptr;
    SkImageInfo info;
    QTimer timer;
    QElapsedTimer lastTime;
};
QSkiaOpenGLWidget::QSkiaOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_dptr(new QSkiaOpenGLWidgetPrivate)
{
    connect(&m_dptr->timer, &QTimer::timeout, this, QOverload<>::of(&QSkiaOpenGLWidget::update));
    m_dptr->timer.start(1000 / qRound(qApp->primaryScreen()->refreshRate()));
}

QSkiaOpenGLWidget::~QSkiaOpenGLWidget()
{
    makeCurrent();
    delete m_dptr;
    m_dptr = nullptr;
    doneCurrent();
}

void QSkiaOpenGLWidget::initializeGL()
{
    m_dptr->funcs.initializeOpenGLFunctions();
    // Skia API change: Use GrDirectContexts::MakeGL() without parameters
    m_dptr->context = GrDirectContexts::MakeGL();
    SkASSERT(m_dptr->context);
    init(this->width(), this->height());
    m_dptr->lastTime.start();
}

void QSkiaOpenGLWidget::resizeGL(int w, int h)
{
    if (this->width() == w && this->height() == h) {
        return;
    }
    init(w, h);
}

void QSkiaOpenGLWidget::init(int w, int h)
{
    m_dptr->info = SkImageInfo::MakeN32Premul(w, h);
    // Skia API change: Use SkSurfaces::RenderTarget with skgpu::Budgeted
    // GrDirectContext inherits from GrRecordingContext, use reinterpret_cast
    m_dptr->gpuSurface = SkSurfaces::RenderTarget(
        reinterpret_cast<GrRecordingContext*>(m_dptr->context.get()),
        skgpu::Budgeted::kNo,
        m_dptr->info
    );
    if (!m_dptr->gpuSurface) {
        qDebug() << "SkSurfaces::RenderTarget return null";
        return;
    }
    m_dptr->funcs.glViewport(0, 0, w, h);
}

void QSkiaOpenGLWidget::paintGL()
{
    if (!this->isVisible()) {
        return;
    }
    if (!m_dptr->gpuSurface) {
        return;
    }
    auto canvas = m_dptr->gpuSurface->getCanvas();
    if (!canvas) {
        return;
    }
    // Qt6 API change: Use QElapsedTimer instead of QTime
    const auto elapsed = m_dptr->lastTime.elapsed();
    m_dptr->lastTime.restart();
    canvas->save();
    this->draw(canvas, elapsed);
    canvas->restore();
}
