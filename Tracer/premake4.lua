-- hdf5 linking is different in windows and ubuntu
hdf5_link = "hdf5_serial"
thread_link = "pthread"
print(os.get())
if os.get() == "windows" then
  hdf5_link = "hdf5"
end
if os.get() == "macosx" then
  hdf5_link = "hdf5"
end




-- A solution contains projects, and defines the available configurations
solution "MyApplication"
   configurations { "Debug", "Release", "Debug_verbose", "ReleaseNoRays" }
   -- A project defines one build target
   project "tracer"

      kind "ConsoleApp"
      language "C++"
      files { "**.h",
              "**.cpp",
              "../raytracer_src/**.cpp",
              "../raytracer_src/**.cpp",
          }

      includedirs {"../raytracer_src/**", "/usr/include/hdf5/serial"}
      excludes{
               "../raytracer_src/Detectors/IrradianceDetector**"}

      configuration "Debug_verbose"
         defines { "DEBUG", "ARMA_DONT_PRINT_ERRORS", "DEBUG_SOLIDPROPAGATION",
                   "DEBUG_VERBOSE", " DEBUG_RAYINTERSECT", "SAVERAYS" }
         links { "armadillo", hdf5_link, thread_link}
         flags { "Symbols" }
         -- Enables some additional warnings.
         buildoptions { "-Wall" , '-W', "-std=c++11"}

      configuration "Debug"
         defines { "DEBUG", "ARMA_DONT_PRINT_ERRORS" , "SAVERAYS"}
         links { "armadillo", hdf5_link, thread_link}
         flags { "Symbols" }
         -- Enables some additional warnings.
         buildoptions { "-Wall" , '-W', "-std=c++11"}


      configuration "Release"
         defines { "NDEBUG", "ARMA_DONT_PRINT_ERRORS", "ARMA_NO_DEBUG" , "SAVERAYS"}
         links { "armadillo", hdf5_link, thread_link}
         flags { "OptimizeSpeed", "EnableSSE", "EnableSSE2"}
         buildoptions {"-std=c++11"}

      configuration "ReleaseNoRays"
         defines { "NDEBUG", "ARMA_DONT_PRINT_ERRORS", "ARMA_NO_DEBUG"}
         links { "armadillo", hdf5_link, thread_link}
         flags { "OptimizeSpeed", "EnableSSE", "EnableSSE2"}
         buildoptions {"-std=c++11"}
         targetname ("tracer-no-ray-save")

-- Installation script
newaction {
   trigger     = "install",
   description = "Install the software",

   execute = function ()
      if not _OPTIONS["DESTDIR"] then
        _OPTIONS["DESTDIR"] = "/usr/local/bin"
      end

      print("######### Installing tracer... #########")
      print("Copying tracer...")
      os.execute(string.format("cp tracer %s/tracer", _OPTIONS["DESTDIR"]))
      os.execute(string.format("cp tracer-no-ray-save %s/tracer-no-ray-save", _OPTIONS["DESTDIR"]))
      print("######### Installation done #########")

   end
}
newoption {
   trigger     = "DESTDIR",
   value       = "koe",
   description = "Output directory for the compiled executable"
}

