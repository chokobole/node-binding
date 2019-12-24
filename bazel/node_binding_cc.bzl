def node_binding_copts():
    return select({
        "@com_github_chokobole_node_binding//:windows": [
            "/std:c++14",
        ],
        "//conditions:default": [
            "-std=c++14",
        ],
    })