project "vane"
    location "vane"
    kind     "StaticLib"
    language "C"

    targetdir (BUILD_BIN_PATH)
    objdir    (BUILD_OBJ_PATH)

    files {
        "vane/src/**.c",
        "vane/include/**.h",
        "vane/include/**.def"
    }

    includedirs { "vane/include" }

project "tests"
    location "tests"
    kind     "ConsoleApp"
    language "C"

    targetdir (BUILD_BIN_PATH)
    objdir    (BUILD_OBJ_PATH)

    files { "tests/src/**.c" }

    includedirs {
        "vane/include",
        DEPENDENCIES_PATH
    }

    links { "vane" }

    debugargs { "--enable-mixed-units" }

project "testbed"
    location "testbed"
    kind     "ConsoleApp"
    language "C"

    targetdir (BUILD_BIN_PATH)
    objdir    (BUILD_OBJ_PATH)

    files { "testbed/src/**.c" }

    includedirs { "vane/include" }

    links { "vane" }