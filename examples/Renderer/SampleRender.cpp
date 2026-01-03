#include "SampleRender.h"

SampleRender::SampleRender()
{
}

void SampleRender::draw(SkCanvas* canvas, int elapsed, int w, int h)
{
    m_rotateAngle = int(elapsed * m_rotateSpeed + m_rotateAngle) % 360;

    canvas->clear(SK_ColorWHITE);
    canvas->rotate(static_cast<SkScalar>(m_rotateAngle), static_cast<SkScalar>(w / 2), static_cast<SkScalar>(h / 2));
    canvas->drawString("Hello Skia", static_cast<SkScalar>(w / 2 - 20), static_cast<SkScalar>(h / 2), m_font, m_paint);
    canvas->drawLine(w * 0.2f, h * 0.2f, w * 0.4f, h * 0.4f, m_paint);
    // Note: flush() removed in new Skia API
}

void SampleRender::init(int w, int h)
{
    m_paint.setAntiAlias(true);
    m_paint.setColor(SK_ColorRED);
    m_paint.setStrokeWidth(2.0f);
    m_font.setSize(30);
}

void SampleRender::resize(int w, int h)
{
}
