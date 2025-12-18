workspace "DX11-Renderer"
	architecture "x64"
	configurations { "Debug", "Release" }
	startproject "Renderer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include"Renderer"