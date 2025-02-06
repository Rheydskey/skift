#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

extern "C" usize zig_main();

Async::Task<> entryPointAsync(Sys::Context& _) {
    usize a = zig_main();

    logInfo("{}", a);
    Sys::println("{}", a);
    co_return Ok();
}
