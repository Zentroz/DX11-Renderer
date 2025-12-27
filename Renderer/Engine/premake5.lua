project "Engine"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files {
        "src/**.h",
        "src/**.cpp"
   }

   includedirs {
       "src",
       "%{wks.location}/Renderer/src",
       "externals/assimp/include",
       "externals/stb",
       "externals/imgui/include"
   }

   links {
       "Renderer",
       "ImGui"
   }

   -- postbuildcommands {
   --     "{COPY} vendor/assimp/lib/assimp-vc143-mt.dll %{cfg.targetdir}"
   -- }


   filter "configurations:Debug"
      runtime "Debug"
      staticruntime "off"

      libdirs {
        "externals/assimp/lib/Debug"
      }

      links {
          "assimp-vc145-mtd"
      }

   filter "configurations:Release"
      runtime "Release"
      staticruntime "off"

      libdirs {
        "externals/assimp/lib/Release"
      }