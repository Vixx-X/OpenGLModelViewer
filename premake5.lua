workspace "OpenGLModelViewer"
    configurations { "Debug", "Release" }
    startproject "OpenGLModelViewer"

    flags { "MultiProcessorCompile" }
    conformancemode "true"

    filter "configurations:Debug"
        defines { "DEBUG", "DEBUG_SHADER" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "Speed"
        flags { "LinkTimeOptimization" }

project "OpenGLModelViewer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    includedirs {
        "src",
        "vendor/glad/include",
        "vendor/glfw/include",
        "vendor/glm/",
        "vendor/imgui",
        "vendor/imgui/backends",
        "vendor/ImGuizmo",
        "vendor/entt/src/entt/",
        "vendor/tinyfiledialogs"
    }

    files { "src/**.cpp", "src/**.h" }

    links { "GLAD", "GLFW", "GLM", "ImGui", "ImGuizmo", "entt", "tinyfiledialogs" }

    filter "system:linux"
        links { "dl", "pthread" }
        defines { "_X11" }

    filter "system:windows"
        defines {
            "_WINDOWS",
            "_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
        }

include "vendor/glad.lua"
include "vendor/glfw.lua"
include "vendor/glm.lua"
include "vendor/imgui.lua"
include "vendor/imguizmo.lua"
include "vendor/entt.lua"
include "vendor/tinyfiledialogs.lua"

