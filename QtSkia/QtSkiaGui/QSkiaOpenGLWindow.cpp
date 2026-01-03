#include "QSkiaOpenGLWindow.h"

#include "core/SkImageInfo.h"
#include "core/SkSurface.h"
#include "core/SkCanvas.h"
#include "gpu/ganesh/SkSurfaceGanesh.h"
// Skia API changes: GrContext moved to ganesh
#include "gpu/ganesh/gl/GrGLDirectContext.h"
#include "gpu/ganesh/GrDirectContext.h"

#include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <QTimer>
#include <QDebug>
class QSkiaOpenGLWindowPrivate {
public:
    QOpenGLFunctions funcs;
    sk_sp<GrDirectContext> context = nullptr;
    sk_sp<SkSurface> gpuSurface = nullptr;
    SkImageInfo info;
    QTimer timer;
    QElapsedTimer lastTime;
    int oldW;
    int oldH;
};
QSkiaOpenGLWindow::QSkiaOpenGLWindow(QWindow* parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , m_dptr(new QSkiaOpenGLWindowPrivate)
{
    connect(&m_dptr->timer, &QTimer::timeout, this, QOverload<>::of(&QSkiaOpenGLWindow::update));
    m_dptr->timer.start(1000 / 60);
}

QSkiaOpenGLWindow::~QSkiaOpenGLWindow()
{
    makeCurrent();
    delete m_dptr;
    m_dptr = nullptr;
    doneCurrent();
}

void QSkiaOpenGLWindow::initializeGL()
{
    m_dptr->funcs.initializeOpenGLFunctions();
    // Skia API change: Use GrDirectContexts::MakeGL() without parameters
    m_dptr->context = GrDirectContexts::MakeGL();
    SkASSERT(m_dptr->context);
    init(this->width(), this->height());
    onInit(this->width(), this->height());
    m_dptr->lastTime.start();
    m_dptr->oldW = width();
    m_dptr->oldW = height();
}

void QSkiaOpenGLWindow::resizeGL(int w, int h)
{
    if (w == m_dptr->oldW && h == m_dptr->oldH) {
        return;
    }
    //TODO 直接换Surface会闪烁，需要优化为快速切换。
    init(w, h);
    onResize(w, h);
}

void QSkiaOpenGLWindow::init(int w, int h)
{
    qWarning() << __FUNCTION__ << w << h;
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

void QSkiaOpenGLWindow::paintGL()
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
    const auto elapsed = m_dptr->lastTime.elapsed();
    m_dptr->lastTime.restart();
    canvas->save();
    this->draw(canvas, elapsed);
    canvas->restore();
}
