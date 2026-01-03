#include "QSkiaWidget.h"

#include "core/SkImageInfo.h"
#include "core/SkSurface.h"
#include "core/SkCanvas.h"
#include "core/SkPixmap.h"
// Skia API changes: GrContext moved to ganesh
#include "gpu/ganesh/GrDirectContext.h"
#include "core/SkData.h"
#include "core/SkImage.h"
#include "core/SkStream.h"

#include <QElapsedTimer>
#include <QTimer>
#include <QResizeEvent>
#include <QPainter>
#include <QImage>
#include <QDebug>

// Use SkSurfaces namespace for WrapPixels
using SkSurfaces::WrapPixels;
class QSkiaWidgetPrivate {
public:
    sk_sp<SkSurface> rasterSurface = nullptr;
    SkImageInfo info;
    QImage image;
    QByteArray data;
    QTimer timer;
    QElapsedTimer lastTime;
};
QSkiaWidget::QSkiaWidget(QWidget* parent)
    : QWidget(parent)
    , m_dptr(new QSkiaWidgetPrivate)
{
    init(this->width(), this->height());
    connect(&m_dptr->timer, &QTimer::timeout, this, QOverload<>::of(&QSkiaWidget::update));
    m_dptr->timer.start(1000 / 60);
}

QSkiaWidget::~QSkiaWidget()
{
    delete m_dptr;
    m_dptr = nullptr;
}

void QSkiaWidget::init(int w, int h)
{
    m_dptr->info = SkImageInfo::Make(w, h, SkColorType::kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
    size_t rowBytes = m_dptr->info.minRowBytes();
    size_t size = m_dptr->info.computeByteSize(rowBytes);
    m_dptr->data.resize(static_cast<int>(size));
    // Skia API change: Use global WrapPixels function
    SkPixmap pixmap(m_dptr->info, m_dptr->data.data(), rowBytes);
    m_dptr->rasterSurface = WrapPixels(pixmap);
    if (!m_dptr->rasterSurface) {
        qDebug() <<"WrapPixels return null";
        return;
    }
}
void QSkiaWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    if (!this->isVisible()) {
        return;
    }
    if (!m_dptr->rasterSurface) {
        return;
    }
    auto canvas = m_dptr->rasterSurface->getCanvas();
    if (!canvas) {
        return;
    }
    // Qt6 API change: Use QElapsedTimer instead of QTime
    const auto elapsed = m_dptr->lastTime.elapsed();
    m_dptr->lastTime.restart();
    canvas->save();
    this->draw(canvas, elapsed);
    canvas->restore();
    QPainter painter(this);
    m_dptr->image = QImage((uchar *)(m_dptr->data.data()), this->width(), this->height(), QImage::Format_RGBA8888);
    if (!m_dptr->image.isNull()) {
        painter.drawImage(0, 0, m_dptr->image);
    }
}

void QSkiaWidget::resizeEvent(QResizeEvent* event)
{
    if (event->oldSize() == event->size()) {
        event->accept();
        return;
    }
    init(event->size().width(), event->size().height());
}

