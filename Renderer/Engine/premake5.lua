project "Engine"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files {
        "src/**.h",
        "src/**.cpp",
        "externals/stb/**.h",
        "externals/stb/**.cpp",
        "externals/entt/include/entt/entt.hpp",
   }

   includedirs {
       "src",
       "%{wks.location}/Renderer/src",
       "externals/assimp/include",
       "externals/stb",
       "externals/entt/include",
       "externals/imgui/include"
   }

   links {
       "Renderer",
       "ImGui"
   }

   filter "configurations:Debug"
      runtime "Debug"
      staticruntime "off"

      libdirs {
        "externals/assimp/lib/Debug"
      }

      links {
          "assimp-vc142-mtd"
      }

   filter "configurations:Release"
      runtime "Release"
      staticruntime "off"

      libdirs {
        "externals/assimp/lib/Release"
      }