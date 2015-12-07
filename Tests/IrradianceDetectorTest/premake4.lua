-- A solution contains projects, and defines the available configurations
solution "MyApplication"
   configurations { "Debug", "Release" }

   -- A project defines one build target
   project "MMP-Raytracer-IrrDetectortest"

      kind "ConsoleApp"
      language "C++"
      files { "**.h",
              "**.cpp",
              "../../raytracer_src/**.cpp",
              "../../raytracer_src/**.cpp",
          }

      includedirs {"../../raytracer_src/**"}

      configuration "Debug"
         defines { "DEBUG" }
         links { "armadillo","tbb"}
         flags { "Symbols" }
         -- Enables some additional warnings.
         buildoptions { "-Wall" , '-W', '-fopenmp'}
	 linkoptions {"-fopenmp"}

      configuration "Release"
         defines { "NDEBUG", "ARMA_DONT_PRINT_ERRORS"  }
         links { "armadillo", "tbb"}
         flags { "OptimizeSpeed", "EnableSSE", "EnableSSE2" }
         buildoptions {"-fopenmp"}
	 linkoptions {"-fopenmp"}
