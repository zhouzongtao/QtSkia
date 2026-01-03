#include "BezierRender.h"
#include "core/SkPathBuilder.h"

void BezierRender::draw(SkCanvas* canvas, int elapsed, int w, int h)
{
    canvas->drawColor(SK_ColorWHITE);
    canvas->drawPath(path, paint);
    // Note: flush() removed in new Skia API - rendering is handled by the surface
}

void BezierRender::init(int w, int h)
{
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(8);
    paint.setColor(0xff4285F4);
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::kRound_Cap);

    // Use SkPathBuilder for new Skia API
    SkPathBuilder builder;
    builder.moveTo(10, 10);
    builder.quadTo(256, 64, 128, 128);
    builder.quadTo(10, 192, 250, 250);
    path = builder.detach();
}

void BezierRender::resize(int w, int h)
{
}
