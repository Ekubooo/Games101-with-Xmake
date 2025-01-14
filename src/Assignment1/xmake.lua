set_project("Rasterizer")

add_rules("mode.debug", "mode.release")

add_requires("opencv", "eigen")

set_languages("cxx17")

if is_os("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_cxflags("/utf-8")
end

target("Assignment1")
    set_kind("binary")
    add_packages("opencv", "eigen")
    add_files("**.cpp")
    add_headerfiles("**.hpp")
    add_includedirs("D:/ComputerScience/Tools/eigen-3.4.0")
    add_includedirs("D:/ComputerScience/Tools")
    set_rundir("$(projectdir)")
