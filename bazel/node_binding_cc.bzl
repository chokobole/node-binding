def node_binding_copts():
    return select({
        "@node_binding//:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
            "-fvisibility=hidden",
        ],
    })