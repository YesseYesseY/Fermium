workspace "Fermium"
    configurations { "Release" }

project "FermiumClient"
    kind "SharedLib"
    language "C++"
    cppdialect "C++23"
    location "Client"
    targetdir "bin"
    objdir "obj/client"
    files {
        "Client/*.cpp",

        "SDK/**.cpp",
        "minhook/src/**.c",
    }
    includedirs {
        "SDK",
        "minhook/include"
    }
    buildoptions { "/wd4369", "/wd4309" }
    architecture "x64"

project "FermiumServer"
    kind "SharedLib"
    language "C++"
    cppdialect "C++23"
    location "Server"
    targetdir "bin"
    objdir "obj/server"
    files {
        "Server/*.cpp",

        "SDK/**.cpp",
        "minhook/src/**.c",
    }
    includedirs {
        "SDK",
        "minhook/include"
    }
    buildoptions { "/wd4369", "/wd4309" }
    architecture "x64"
