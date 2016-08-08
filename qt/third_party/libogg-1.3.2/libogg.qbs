import qbs

StaticLibrary {
    name:"ogg"

    Depends { name: "cpp" }

    files : [
        "src/*.c"
    ]
    cpp.includePaths : ['include']

    Export {
        Depends { name: "cpp" }
        cpp.includePaths : ['include']
    }
}
