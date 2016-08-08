import qbs

StaticLibrary {
    name:"vorbis"

    Depends { name: "cpp" }
    Depends { name: "ogg" }

    Group {
        files : [
            "mdct.c", "smallft.c", "block.c", "envelope.c", "window.c", "lpc.c", "analysis.c",
            "synthesis.c", "psy.c", "info.c", "floor1.c", "floor0.c", "res0.c","mapping0.c",
            "registry.c", "codebook.c", "sharedbook.c", "lookup.c", "bitrate.c", "envelope.h",
            "lpc.h", "lsp.c", "codebook.h", "misc.h", "psy.h", "masking.h", "os.h", "mdct.h",
            "smallft.h", "highlevel.h", "registry.h", "scales.h", "window.h",
            "lookup.h", "lookup_data.h", "codec_internal.h", "backends.h", "bitrate.h",
            "vorbisfile.c"
        ]
        prefix: "lib/"
    }

    cpp.includePaths : ['include']
    cpp.linkerFlags : [ "-no-undefined" ]

    Export {
        Depends { name: "cpp" }
        Depends { name: "ogg" }
        cpp.includePaths : ['include']
    }
}
