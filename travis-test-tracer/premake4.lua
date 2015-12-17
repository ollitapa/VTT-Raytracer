-- hdf5 linking is different in windows and ubuntu
hdf5_link = "hdf5"
thread_link = "pthread"

-- Check current linux distro, (on windows returns mingw)
local handle = io.popen('uname -a')
local result = handle:read("*a")
handle:close()

if string.find(result, 'Debian') then
  print(result)
  hdf5_link="hdf5_serial"
end



--  ugly hack to use gcc-4.8
premake.gcc.cc  = 'gcc-4.8'
premake.gcc.cxx = 'g++-4.8'


-- A solution contains projects, and defines the available configurations
solution "MyApplication"
   configurations { "Debug", "Release", "Debug_verbose", "ReleaseNoRays" }
   -- A project defines one build target
   project "tracer"

      kind "ConsoleApp"
      language "C++"
      files { "**.h",
              "**.cpp",
              "../tracer.cpp",
              "../raytracer_src/**.cpp",
              "../raytracer_src/**.cpp",
          }


      includedirs {"../raytracer_src/**", "/usr/include/hdf5/serial"}

      includedirs {"../raytracer_src/**",
                   "../arma/usr/",
                   "../arma/usr/include/",
                   "../arma/usr/include/armadillo_bits",
                   "../arma/usr/share/Armadillo/CMake",
                   "/usr/include/hdf5/serial"}
      libdirs {
          "../arma/usr/lib/"
          }
      excludes{
               "../raytracer_src/Detectors/IrradianceDetector**",
               "../Tests/IrradianceDetectorTest**"}

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


--
-- Rename gmake action into _gmake
--
assert(premake.action.list.gmake, "Your Premake does not have gmake action!")
premake.action.list.gmake, premake.action.list._gmake = nil, premake.action.list.gmake
premake.action.list._gmake.trigger = "_gmake"

--
-- Add new gmake action
--
newaction {
    trigger     = "gmake",
    description = premake.action.get("_gmake").description,
    shortname   = premake.action.get("_gmake").shortname,
    valid_kinds = premake.action.get("_gmake").valid_kinds,
    valid_languages = premake.action.get("_gmake").valid_languages,
    valid_tools = premake.action.get("_gmake").valid_tools,

    onproject = function(prj)
       -- Your code
    end,

    execute = function()
        -- Your code
        premake.action.call("_gmake")
        -- Opens a file in append mode
        file = io.open("Makefile", "a")
        io.output(file)
        io.write("\n")
        io.write("INSTALL = install -c\n")
        io.write("prefix = /usr/local\n")
        io.write("binprefix = \n")
        io.write("# The directory to install tar in.\n")
        io.write("bindir = $(prefix)/bin\n")
        io.write("install:\n")
        io.write("ifeq ($(config),releasenorays)\n")
        io.write("\t$(INSTALL) tracer-no-ray-save $(DESTDIR)$(bindir)/$(binprefix)tracer-no-ray-save\n")
        io.write("else\n")
        io.write("\t$(INSTALL) tracer $(DESTDIR)$(bindir)/$(binprefix)tracer\n")
        io.write("endif\n")
        io.write("\n")

        -- closes the open file
        io.close(file)
    end
}
