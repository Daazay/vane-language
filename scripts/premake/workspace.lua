workspace "vane-language"
    location (ROOT_PATH)
    startproject "testbed"

    -- general
    systemversion "latest"
    warnings      "Extra"
    cdialect      "C17"

    --
    configurations { "debug", "release" }
    platforms { "x64" }

    -- configuration filters
    filter "configurations:debug"
        symbols  "on"
        optimize "off"
        runtime  "debug"

    filter"configurations:release"
        symbols  "off"
        optimize "on"
        runtime  "release"

    filter "system:windows"
        defines { "PLATFORM_WINDOWS" }

    filter "system:linux"
        defines { "PLATFORM_LINUX" }