set_project("BezierCurve")

add_rules("mode.debug", "mode.release")

add_requires("opencv")

set_languages("cxx14")

if is_os("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_cxflags("/utf-8")
end

target("BezierCurve")
    set_kind("binary")
    add_packages("opencv")
    add_files("*.cpp")
    add_headerfiles("*.hpp")
    add_includedirs("D:/ComputerScience/Tools")
    add_includedirs("D:/ComputerScience/Tools/eigen-3.4.0")
    set_rundir("$(projectdir)")