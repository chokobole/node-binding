load(
    "//:node_addon_api.bzl",
    "NODE_VERSION",
    "node_addon_api_copts",
    "node_addon_api_defines",
)

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "x86_64",
    constraint_values = ["@bazel_tools//platforms:x86_64"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "node_addon_api",
    hdrs = [
        "node_modules/node-addon-api/napi.h",
        "node_modules/node-addon-api/napi-inl.deprecated.h",
        "node_modules/node-addon-api/napi-inl.h",
        "node_modules/node-addon-api/src/node_api.h",
        "node_modules/node-addon-api/src/node_api_types.h",
        "node_modules/node-addon-api/src/node_internals.h",
        "node_modules/node-addon-api/src/util.h",
        "node_modules/node-addon-api/src/util-inl.h",
    ],
    copts = node_addon_api_copts(),
    defines = node_addon_api_defines(),
    includes = [
        "node_modules/node-addon-api",
        "node_modules/node-addon-api/src",
    ],
    visibility = ["//visibility:public"],
    deps = [":node"],
)

cc_import(
    name = "node_win_lib",
    interface_library = select({
        "//:x86_64": "%s/x64/node.lib" % (NODE_VERSION),
        "//conditions:default": "%s/ia32/node.lib" % (NODE_VERSION),
    }),
    system_provided = 1,
)

cc_library(
    name = "node",
    hdrs = glob(["%s/include/**/*.h" % (NODE_VERSION)]),
    includes = [
        "%s/include" % (NODE_VERSION),
        "%s/include/node" % (NODE_VERSION),
    ],
    linkopts = select({
        "//:windows": [
            "DelayImp.lib",
            "/DELAYLOAD:\"node.exe\"",
        ],
        "//conditions:default": [],
    }),
    deps = select({
        "//:windows": [":node_win_lib"],
        "//conditions:default": [],
    }),
)
