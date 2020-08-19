{
  "targets": [
    {
      "target_name": "6_stl",
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "cflags_cc": ["-std=c++17", "-frtti", "-fexceptions"], # -std=c++14 or later
      "sources": ["addon.cc"],
      "include_dirs": [
        "<!@(node -p \"require('../../').include\")"
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES", # optional
        "GCC_ENABLE_CPP_RTTI": "YES", # optional
        "CLANG_CXX_LANGUAGE_STANDARD":"c++17", # c++14 or later
        "MACOSX_DEPLOYMENT_TARGET": "10.14" # 10.12 or later
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1, # optional
          "RuntimeTypeInfo": "true", # optional
          "AdditionalOptions": ["-std:c++17"] # -std:c++14 or later
        }
      },
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
    }
  ]
}