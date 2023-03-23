#include <karm-base/ring.h>
#include <karm-math/rand.h>

#include "filters.h"

namespace Karm::Gfx {

struct StackBlur {
    isize _radius;
    Ring<Math::Vec4u> _queue;
    Math::Vec4u _sum;

    StackBlur(isize radius)
        : _radius(radius), _queue(width()) {
        clear();
    }

    Math::Vec4u outgoingSum() const {
        Math::Vec4u sum = {};
        for (isize i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(i);
        }
        return sum;
    }

    Math::Vec4u incomingSum() const {
        Math::Vec4u sum = {};
        for (isize i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(width() - i - 1);
        }
        return sum;
    }

    isize width() const {
        return _radius * 2 + 1;
    }

    isize denominator() const {
        return _radius * (_radius + 2) - 1;
    }

    void enqueue(Math::Vec4u color) {
        _queue.pushBack(color);
        _sum = _sum + incomingSum() - outgoingSum();
    }

    Math::Vec4u dequeue() {
        auto res = _sum / denominator();
        _queue.dequeue();
        return res;
    }

    void clear() {
        _sum = {};
        _queue.clear();
        for (isize i = 0; i < width(); i++) {
            _queue.pushBack({});
        }
    }
};

[[gnu::flatten]] void BlurFilter::apply(Surface s) const {
    StackBlur stack{amount};
    auto b = s.bound();

    for (isize y = b.top(); y < b.bottom(); y++) {
        for (isize i = 0; i < stack.width(); i++) {
            auto x = b.start() + i - amount;
            stack.dequeue();
            stack.enqueue(s.loadClamped({x, y}));
        }

        for (isize x = b.start(); x < b.end(); x++) {
            s.store({x, y}, stack.dequeue());
            stack.enqueue(s.loadClamped({x + amount + 1, y}));
        }

        stack.clear();
    }

    for (isize x = b.start(); x < b.end(); x++) {
        for (isize i = 0; i < stack.width(); i++) {
            isize const y = b.top() + i - amount;
            stack.dequeue();
            stack.enqueue(s.loadClamped({x, y}));
        }

        for (isize y = b.top(); y < b.bottom(); y++) {
            s.store({x, y}, stack.dequeue());
            stack.enqueue(s.loadClamped({x, y + amount + 1}));
        }

        stack.clear();
    }
}

void SaturationFilter::apply(Surface s) const {
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = s.load({b.x + x, b.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            auto gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;

            u8 red = min(gray * amount + color.red * (1 - amount), 255);
            u8 green = min(gray * amount + color.green * (1 - amount), 255);
            u8 blue = min(gray * amount + color.blue * (1 - amount), 255);

            color = Color::fromRgba(red, green, blue, color.alpha);

            s.store({b.x + x, b.y + y}, color);
        }
    }
}

void GrayscaleFilter::apply(Surface s) const {
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = s.load({b.x + x, b.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            f64 gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;
            color = Color::fromRgba(gray, gray, gray, color.alpha);
            s.store({b.x + x, b.y + y}, color);
        }
    }
}

void ContrastFilter::apply(Surface s) const {
    auto b = s.bound();
    f64 factor = (259 * ((amount * 255) + 255)) / (255 * (259 - (amount * 255)));

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = s.load({b.x + x, b.y + y});

            color = Color::fromRgba(
                min(factor * (color.red - 128) + 128, 255),
                min(factor * (color.green - 128) + 128, 255),
                min(factor * (color.blue - 128) + 128, 255),
                color.alpha);

            s.store({b.x + x, b.y + y}, color);
        }
    }
}

void BrightnessFilter::apply(Surface s) const {
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = s.load({b.x + x, b.y + y});

            color = Color::fromRgba(
                min(color.red * amount, 255),
                min(color.green * amount, 255),
                min(color.blue * amount, 255),
                color.alpha);

            s.store({b.x + x, b.y + y}, color);
        }
    }
}

void NoiseFilter::apply(Surface s) const {
    Math::Rand rand{0x12341234};
    u8 alpha = 255 * amount;
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            u8 noise = rand.nextU8();

            s.blend(
                {b.x + x, b.y + y},
                Color::fromRgba(noise, noise, noise, alpha));
        }
    }
}

void SepiaFilter::apply(Surface s) const {
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = s.load({b.x + x, b.y + y});

            auto sepiaColor = Color::fromRgba(
                min((color.red * 0.393) + (color.green * 0.769) + (color.blue * 0.189), 255u),
                min((color.red * 0.349) + (color.green * 0.686) + (color.blue * 0.168), 255u),
                min((color.red * 0.272) + (color.green * 0.534) + (color.blue * 0.131), 255u),
                color.alpha);

            s.store({b.x + x, b.y + y}, color.lerpWith(sepiaColor, amount));
        }
    }
}

void TintFilter::apply(Surface s) const {
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = s.load({b.x + x, b.y + y});

            auto tintColor = Color::fromRgba(
                (color.red * tint.red) / 255,
                (color.green * tint.green) / 255,
                (color.blue * tint.blue) / 255,
                (color.alpha * tint.alpha) / 255);

            s.store({b.x + x, b.y + y}, tintColor);
        }
    }
}

void OverlayFilter::apply(Surface s) const {
    auto b = s.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            s.blend(
                {b.x + x, b.y + y},
                color);
        }
    }
}

void FilterChain::apply(Surface s) const {
    filters.visit([&](auto &f) {
        f->apply(s);
    });
}

} // namespace Karm::Gfx
