project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "include/imgui/**.h",
        "include/imgui/**.cpp",
    }

    includedirs {
        "include/imgui"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"        -- MDd
        staticruntime "off"

    filter "configurations:Release"
        runtime "Release"      -- MD
        staticruntime "off"