project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    architecture "x86_64"

    targetdir "../bin/%{cfg.buildcfg}"
    objdir "../obj/%{cfg.buildcfg}"

    includedirs { "ImGuizmo/", "imgui" }

    files
    {
        "ImGuizmo/*.cpp",
    }

    links { "GLFW", "ImGui" }

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
