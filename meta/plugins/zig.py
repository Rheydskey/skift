from cutekit.rules import Rule, append
from cutekit.model import DEFAULT_TOOLS, Tool

append(
    Rule(
        "zig",
        ["*.zig"],
        "*.o.o",
        "build-obj $cincs $cdefs $flags -femit-bin=$out $in",
        [
            "-fPIE",
            "-fno-stack-check",
            "-target", "x86_64-freestanding-none"
        ],
    )
)


DEFAULT_TOOLS["zig"] = Tool("zig")
