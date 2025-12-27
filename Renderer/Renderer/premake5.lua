project "Renderer"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files {
        "src/**.h",
        "src/**.cpp"
   }

   includedirs {
       "src/Renderer"
   }

   filter "configurations:Debug"
      runtime "Debug"
      staticruntime "off"

   filter "configurations:Release"
      runtime "Release"
      staticruntime "off"