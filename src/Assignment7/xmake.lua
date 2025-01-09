set_project("RayTracing")

add_rules("mode.debug", "mode.release")

set_languages("cxx17")

set_warnings("all")

if is_os("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_cxflags("/utf-8")
end

target("RayTracing")
    set_kind("binary")
    add_files("*.cpp")
    add_headerfiles("*.hpp")
    add_cxflags("-fopenmp")
    add_ldflags("-fopenmp")
    add_includedirs("D:/ComputerScience/Tools")
    add_includedirs("D:/ComputerScience/Tools/eigen-3.4.0")
    set_rundir("$(projectdir)")
