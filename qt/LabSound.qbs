import qbs

StaticLibrary {
    name:"LabSound"

    Depends { name: "cpp" }
    Depends { name: "Qt.multimedia" }

    cpp.cxxLanguageVersion : "c++11"
    cpp.minimumOsxVersion : "10.7"

//    Depends { name: "nyquist" }

    Group {
        name: "core"
        files : [ "*.cpp" ]
        prefix : "../src/core/"
    }
    Group {
        name: "internal"
        files : [ "*.cpp" ]
        prefix : "../src/internal/src/"
        excludeFiles: [
            "AudioFileReader.cpp"
        ]
    }
    /*Group {
        name: "test"
        files : [ "*.cpp" ]
        prefix : "../src/internal/src/win/"
    }*/
    Group {
        name: "extended"
        files : [ "*.cpp" ]
        prefix : "../src/extended/"

        excludeFiles: [
            "RecorderNode.cpp"
        ]
    }

    Group {
        name: "kissfft"
        files : [ "*.cpp" ]
        prefix : "../third_party/kissfft/src/"
    }

    Group {
        name: "ooura"
        files : [ "*.cpp" ]
        prefix : "../third_party/ooura/src/"
    }

    Group {
        name: "Qt"
        files  : [ "*.cpp", "*.h"]
        prefix : "./"
    }

    Depends { name: "Qt.multimedia" }

    cpp.includePaths : ['../include', '../src', '../third_party']
    cpp.defines : [ 'WTF_USE_WEBAUDIO_KISSFFT' ]

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt.multimedia" }
        cpp.includePaths : ['../include', '../src', '../third_party']

        Properties {
            condition: qbs.targetOS.contains("osx")
            cpp.frameworks: ["Accelerate"]
        }
    }
}
