cmake_minimum_required(VERSION 3.14)

project(codeedit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)


# set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS        OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)


find_package(Boost REQUIRED COMPONENTS json regex)

pkg_check_modules(gtkmm4 REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(sigc REQUIRED IMPORTED_TARGET sigc++-3.0)

add_executable(
	codeedit 
	CommonEditorWindow.cpp
	Controller.cpp
	EditorListView.cpp
	FileExplorer.cpp
	FindFile.cpp
	FindText.cpp
	main.cpp
	modularity.cpp
	ProjectCtl.cpp
	ProjectCtlWin.cpp
	ProjectMgr.cpp
	ProjectMgrEditor.cpp
	tables.cpp
	utils.cpp
	WorkSubject.cpp
	WorkSubjectListView.cpp
	builtin_mods/ccpp/CodeEditorCCPP.cpp
	builtin_mods/ccpp/mod_ccpp.cpp
    )

target_link_libraries(
    codeedit 
    PUBLIC 
    # -static
    PkgConfig::gtkmm4 
    PkgConfig::sigc 
    # PkgConfig::gtkmm3 
    # PkgConfig::gtksourceviewmm 
    Boost::json
    ${Boost_LIBRARIES}
    )

target_include_directories(
    codeedit 
    PUBLIC 
    ${Boost_INCLUDE_DIRS}
    )

