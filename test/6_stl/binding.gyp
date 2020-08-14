{
  "targets": [
    {
      "target_name": "6_stl",
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "cflags_cc": ["-std=c++17", "-frtti", "-fexceptions"],
      "sources": ["addon.cc"],
      "include_dirs": [
        "<!@(node -p \"require('../../').include\")"
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES", # optional
        "GCC_ENABLE_CPP_RTTI": "YES", # optional
        "CLANG_CXX_LANGUAGE_STANDARD":"c++17",
        "MACOSX_DEPLOYMENT_TARGET": "10.14"
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1, # optional
          "RuntimeTypeInfo": "true", # optional
          "AdditionalOptions": ["-std:c++17"]
        }
      },
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
    }
  ]
}