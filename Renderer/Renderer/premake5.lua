project "Renderer"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files {
        "src/**.h",
        "src/**.cpp"
   }

   includedirs {
       "src"
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   -- filter "configurations:Debug"
   --      runtime "Debug"        -- MDd
   --      staticruntime "off"
   -- 
   -- filter "configurations:Release"
   --      runtime "Release"      -- MD
   --      staticruntime "off"