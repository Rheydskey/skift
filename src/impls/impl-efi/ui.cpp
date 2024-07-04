#include <efi/base.h>
#include <karm-ui/drag.h>

#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

struct EfiHost :
    public Host {
    Efi::SimpleTextInputProtocol *_stip = nullptr;
    Gfx::MutPixels _front;
    Strong<Gfx::Surface> _back;

    EfiHost(
        Child root,
        Efi::SimpleTextInputProtocol *stip,
        Gfx::MutPixels front,
        Strong<Gfx::Surface> back
    )
        : Host(root),
          _stip(stip),
          _front(front),
          _back(back) {
        _dirty.pushBack(front.bound());
    }

    Gfx::MutPixels mutPixels() override {
        return *_back;
    }

    void flip(Slice<Math::Recti> dirty) override {
        for (auto d : dirty)
            Gfx::blitUnsafe(_front.clip(d), mutPixels());
    }

    Res<> wait(TimeStamp) override {
        usize eventIdx = {};
        Efi::bs()->waitForEvent(1, &_stip->waitForKey, &eventIdx).unwrap();
        Efi::Key key = {};
        _stip->readKeyStroke(_stip, &key).unwrap();
        auto e = key.toKeyEvent();
        event<Events::KeyboardEvent>(*this, e);
        return Ok();
    }

    void bubble(Sys::Event &e) override {
        Host::bubble(e);
    }
};

Res<Strong<Host>> makeHost(Child root) {
    auto *stip = try$(Efi::locateProtocol<Efi::SimpleTextInputProtocol>());
    auto *gop = try$(Efi::locateProtocol<Efi::GraphicsOutputProtocol>());
    auto *mode = gop->mode;

    logInfo("efi: gop: {}x{}, {} stride, {} modes", mode->info->horizontalResolution, mode->info->verticalResolution, mode->info->pixelsPerScanLine * 4, mode->maxMode);
    Gfx::MutPixels front = {
        (void *)mode->frameBufferBase,
        {(u16)mode->info->horizontalResolution, (u16)mode->info->verticalResolution},
        (u16)(mode->info->pixelsPerScanLine * 4),
        Gfx::BGRA8888,
    };

    auto back = Gfx::Surface::alloc({front.width(), front.height()}, Gfx::BGRA8888);

    return Ok(makeStrong<EfiHost>(root, stip, front, back));
}

} // namespace Karm::Ui::_Embed
