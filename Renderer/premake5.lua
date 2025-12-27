workspace "DX11-Renderer"
	architecture "x64"
	configurations { "Debug", "Release" }
	startproject "Engine"

	filter "configurations:Debug"
        runtime "Debug"
        staticruntime "off"

    filter "configurations:Release"
        runtime "Release"
        staticruntime "off"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include"Renderer"
include"Engine"

group "Externals"
	include"Engine/externals/imgui"