
project "yaml-cpp"
    kind "StaticLib"
    language "C++"

    targetdir "../bin/%{cfg.buildcfg}"
    objdir "../obj/%{cfg.buildcfg}"

    files
    {
        "yaml-cpp/src/**.h",
        "yaml-cpp/src/**.cpp",

        "yaml-cpp/include/**.h"
    }

    includedirs
    {
        "yaml-cpp/include"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

    filter "system:linux"
        pic "On"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
