NODE_VERSION=%{NODE_VERSION}

def node_addon_api_copts():
    return select({
        "//:windows": [
            "/GS",
            "/DWIN32",
            "/D_CRT_SECURE_NO_DEPRECATE",
            "/D_CRT_NONSTDC_NO_DEPRECATE",
            "/GR-",
            "/GF",
            "/D_HAS_EXCEPTIONS=0",
        ],
        "//conditions:default": [
            "-D_LARGEFILE_SOURCE",
            "-D_FILE_OFFSET_BITS=64",
            "-fvisibility=hidden",
            "-fno-rtti",
            "-fno-exceptions"
        ],
    })

def node_addon_api_defines():
    return [
        "EXTERNAL_NAPI",
        "NAPI_DISABLE_CPP_EXCEPTIONS",
    ] + select({
        "//:windows": [
            "USING_UV_SHARED=1",
            "USING_V8_SHARED=1",
            "V8_DEPRECATION_WARNINGS=1",
            "BUILDING_NODE_EXTENSION",
        ],
        "//conditions:default": [],
    })
