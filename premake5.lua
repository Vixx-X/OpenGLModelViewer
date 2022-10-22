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
        "vendor/glm",
        "vendor/imgui",
        "vendor/imgui/backends",
        "vendor/imgui/examples",
        "vendor/ImGuizmo",
        "vendor/entt/single_include/entt",
        "vendor/libtinyfiledialogs",
        "vendor/yaml-cpp/include"
    }

    files {
        "src/**.cpp",
        "src/**.h",

        -- Could not compile and link yaml-cpp on windows so ill be embbeding it into my engine
        "vendor/yaml-cpp/src/**.cpp",
        "vendor/yaml-cpp/src/**.h",
        "vendor/yaml-cpp/include/**.h"
    }

    links { "GLAD", "GLFW", "GLM", "ImGui", "ImGuizmo", "tinyfiledialogs" }

    filter "system:linux"
        links { "dl", "pthread" }
        defines { "_X11" }

    filter "system:windows"
    links { "opengl32.lib" }
        defines {
            "_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "GLFW_INCLUDE_NONE",
            "YAML_CPP_STATIC_DEFINE"
        }

include "vendor/glad.lua"
include "vendor/glfw.lua"
include "vendor/glm.lua"
include "vendor/imgui.lua"
include "vendor/imguizmo.lua"
include "vendor/tinyfiledialogs.lua"
