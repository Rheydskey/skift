#pragma once

#include <karm-media/image.h>

#include "base.h"

namespace Web::Layout {

struct ImageFrag : public Frag {
    static constexpr auto TYPE = IMAGE;

    Media::Image _image;

    ImageFrag(Strong<CSSOM::Computed> style, Media::Image image)
        : Frag(style), _image(image) {
    }

    Type type() const override {
        return TYPE;
    }

    void layout(Types::RectPx) override {
    }

    Types::Vec2Px size(Types::Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
