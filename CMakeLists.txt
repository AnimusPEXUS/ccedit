cmake_minimum_required(VERSION 3.14)

project(codeedit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(PkgConfig REQUIRED)


# set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS        OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)


find_package(Boost REQUIRED COMPONENTS json)
# find_package(Boost REQUIRED COMPONENTS process)

#pkg_check_modules(gtkmm3 REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(gtkmm4 REQUIRED IMPORTED_TARGET gtkmm-4.0)
#pkg_check_modules(gtksourceviewmm REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)

add_executable(
	codeedit 
	CommonEditorWindow.cpp
	Controller.cpp
	EditorListView.cpp
	FileExplorer.cpp
	main.cpp
	modularity.cpp
	ProjectCtl.cpp
	ProjectMgr.cpp
	ProjectMgrEditor.cpp
	WorkSubject.cpp
	WorkSubjectListView.cpp
	utils.cpp
	builtin_mods/ccpp/module_ccpp.cpp
    )

target_link_libraries(
    codeedit 
    PUBLIC 
    # -static
    PkgConfig::gtkmm4 
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

