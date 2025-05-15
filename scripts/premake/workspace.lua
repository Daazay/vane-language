workspace "vane-compiler"
    location (ROOT_PATH)
    startproject "testbed"

    -- general
    systemversion "latest"
    warnings      "Extra"
    cdialect      "C17"

    --
    configurations { "debug", "release" }
    platforms { "x64" }

    --
    newoption {
        trigger = "cc",
        value   = "COMPILER",
        description = "Choose compiler (gcc/clang)",
        allowed = {
            -- make
            { "gcc",      "GNU Compiler Collection" },
            { "clang",    "LLVM Clang Compiler" },
            -- vs compilers
            { "clang-cl", "Clang for MSVC compatibility" },
            { "msc",      "	Microsoft C/C++ compiler" }
        }
    }

    if _OPTIONS["cc"] then
        if _OPTIONS["cc"] == "clang" and os.target() == "windows" then
            toolset "clang-cl"
        else
            toolset(_OPTIONS["cc"])
        end
    end

    -- filters
    filter "configurations:debug"
        symbols  "on"
        optimize "off"
        runtime  "debug"

    filter"configurations:release"
        symbols  "off"
        optimize "on"
        runtime  "release"

    filter "toolset:gcc"
        buildoptions { "-Wall", "-Wextra" }

    filter "toolset:clang"
        buildoptions { "-Weverything", "-Wno-declaration-after-statement" }

    filter "system:windows"
        defines { "PLATFORM_WINDOWS" }

    filter "system:linux"
        defines { "PLATFORM_LINUX" }