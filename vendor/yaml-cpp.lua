project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
    architecture "x86_64"

    targetdir "../bin/%{cfg.buildcfg}"
    objdir "../obj/%{cfg.buildcfg}"

    defines {
        "YAML_CPP_STATIC_DEFINE"
    }

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

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"