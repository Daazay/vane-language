project "vane"
    location "."
    kind     "StaticLib"
    language "C"

    targetdir (BUILD_BIN_PATH)
    objdir    (BUILD_OBJ_PATH)

    files {
        "src/**.c",
        "include/**.h",
        "include/**.def"
    }

    includedirs {
        "include"
    }