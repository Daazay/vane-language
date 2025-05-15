project "testbed"
    location "."
    kind     "ConsoleApp"
    language "C"

    targetdir (BUILD_BIN_PATH)
    objdir    (BUILD_OBJ_PATH)

    files {
        "src/**.c"
    }

    includedirs {
        path.join(PROJECTS_PATH, "vane/include")
    }

    links { "vane" }