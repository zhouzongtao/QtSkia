#include "SkWidget.h"
#include "core/SkCanvas.h"
#include "core/SkColor.h"
#include "core/SkPaint.h"
#include "core/SkRRect.h"
#include "core/SkFont.h"

void SkWidget::draw(SkCanvas* canvas, int elapsed)
{
    m_rotateAngle = int(elapsed * m_rotateSpeed + m_rotateAngle) % 360;
    canvas->clear(SK_ColorWHITE);
    SkRect rect = SkRect::Make(SkISize::Make(this->width(), this->height()));
    SkRRect rrect = SkRRect::MakeRectXY(rect, static_cast<SkScalar>(this->width() / 2), static_cast<SkScalar>(this->height() / 4));
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setARGB(128, 51, 76, 102);
    canvas->translate(static_cast<SkScalar>(x), static_cast<SkScalar>(y));
    canvas->drawRRect(rrect, paint);

    SkFont font;
    font.setSize(30);
    paint.setColor(SK_ColorRED);
    canvas->rotate(static_cast<SkScalar>(m_rotateAngle), static_cast<SkScalar>(this->width() / 2), static_cast<SkScalar>(this->height() / 2));
    canvas->drawString("Hello Skia", 600, 300, font, paint);

    // Note: flush() removed in new Skia API
}
