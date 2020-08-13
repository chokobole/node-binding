{
  "targets": [
    {
      "target_name": "6_stl",
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "cflags_cc": ["-std=c++17", "-frtti"],
      "sources": ["addon.cc"],
      "include_dirs": [
        "<!@(node -p \"require('../../').include\")",
      ],
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1,
          "AdditionalOptions": ["-std:c++17"]
        }
      },
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
    }
  ]
}