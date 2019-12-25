exports_files(["LICENSE"])

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "node_binding",
    hdrs = [
        "node_binding/arg_type_checker.h",
        "node_binding/constructor.h",
        "node_binding/macros.h",
        "node_binding/stl.h",
        "node_binding/template_util.h",
        "node_binding/type_convertor.h",
        "node_binding/typed_call.h",
    ],
    deps = [
        "@node_addon_api",
    ],
    visibility = ["//visibility:public"],
)
