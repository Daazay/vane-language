project "tests"
    location "."
    kind     "ConsoleApp"
    language "C"

    targetdir (BUILD_BIN_PATH)
    objdir    (BUILD_OBJ_PATH)

    files {
        "src/**.c"
    }

    includedirs {
        path.join(PROJECTS_PATH, "vane/include"),
        DEPENDENCIES_PATH
    }

    links { "vane" }

    debugargs { "--enable-mixed-units" }