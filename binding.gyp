{
  "targets": [{

      "target_name": "orbidx",

      "sources": [
        "src/init.cc",
        "src/orbindexer.cc",
        "src/orbwordindex.cc",
      ],

      "libraries": [
        "<!@(node utils/find-opencv.js --libs)"
      ],
      # For windows

      "include_dirs": [
        "<!@(node utils/find-opencv.js --cflags)",
        "<!(node -e \"require('nan')\")"
      ],

      "cflags!" : [ "-fno-exceptions"],
      "cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"],

      "conditions": [
        [ "OS==\"linux\" or OS==\"freebsd\" or OS==\"openbsd\" or OS==\"solaris\" or OS==\"aix\"", {
            "cflags": [
              "<!@(node utils/find-opencv.js --cflags)",
              "-Wall"
            ]
        }],
        [ "OS==\"win\"", {
            "cflags": [
              "-Wall"
            ],
            "defines": [
                "WIN"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": "2",
                "DisableSpecificWarnings": [ "4530", "4506", "4244" ],
              },
            }
        }],
        [ # cflags on OS X are stupid and have to be defined like this
          "OS==\"mac\"", {
            "xcode_settings": {
            "OTHER_CFLAGS": [
              "-mmacosx-version-min=10.7",
            "-std=c++11",
            "-stdlib=libc++",
            "<!@(node utils/find-opencv.js --cflags)",
              ],
            "GCC_ENABLE_CPP_RTTI": "YES",
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
          }
        }]
    ]
  }
 ]
}
