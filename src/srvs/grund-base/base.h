#pragma once
// Generated by chatty from src/srvs/grund-base/base.idl
// DO NOT EDIT
#include <karm-ipc/base.h>
namespace Grund {

struct ILocator {
    static constexpr auto _UID = 0x626685c968676fc2;
    static constexpr auto _NAME = "Locator";

    template <typename T>
    struct _Client;

    template <typename R>
    auto _dispatch(R &r);

    virtual ~ILocator() = default;
    virtual Ipc::AsyncRes<> queryInterface(u64 id) = 0;
};

template <typename T>
struct ILocator::_Client : public ILocator {
    T _t;

    _Client(T t) : _t{t} {}

    Ipc::AsyncRes<> queryInterface(u64 id) {
        return _t.template invoke<ILocator, 0x211939d80e187993, Ipc::AsyncRes<>(u64)>(id);
    }
};

template <typename R>
auto ILocator::_dispatch(R &r) {
    switch (r.id()) {

    case 0x211939d80e187993: {
        return r.reply(queryInterface(r.template get<u64>()));
    }

    default:
        return r.error();
    }
}

} // namespace Grund
