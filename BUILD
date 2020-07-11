package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "allocator",
    srcs = glob([
        "lib/allocator/*.cpp"
    ]),
    hdrs = glob([
        "lib/allocator/*.h"
    ]),
    includes = [
        "lib/allocator"
    ]
)

cc_library(
    name = "iterator",
    hdrs = glob([
        "lib/iterator/*.hpp"
    ]),
    includes = [
        "lib/iterator"
    ]
)

cc_library(
    name = "constructor",
    hdrs = glob([
        "lib/constructor/*.hpp"
    ]),
    includes = [
        "lib/constructor"
    ]
)

cc_library(
    name = "vector",
    srcs = glob([
        "lib/vector/*.cpp"
    ]),
    hdrs = glob([
        "lib/vector/*.h"
    ]),
    deps = [
        "//:allocator",
        "//:iterator",
        "//:constructor"
    ],
    includes = [
        "lib/vector"
    ]
)

cc_binary(
    name = "main",
    srcs = [
        "src/main.cc"
    ],
    deps = [
        "//:allocator",
        "//:iterator",
        "//:vector",
    ],
    linkopts = [
        "-lpthread",
    ]
)
