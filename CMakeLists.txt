cmake_minimum_required(VERSION 3.30.5)

project(ccedit)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

# set(Boost_USE_STATIC_LIBS      ON)
set(Boost_USE_DEBUG_LIBS        OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)


find_package(Boost REQUIRED COMPONENTS json regex)

pkg_check_modules(gtk4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(gtkmm4 REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(sigc REQUIRED IMPORTED_TARGET sigc++-3.0)

find_package(ICU 73.0 COMPONENTS uc i18n REQUIRED)

set(ICU_DEBUG OFF)

add_executable(
  ccedit

  CommonEditorWindow.cpp
  Controller.cpp
  FileExplorer.cpp
  FileExplorerTables.cpp
  FindFile.cpp
  FindTables.cpp
  FindText.cpp
  FindTypes.cpp
  main.cpp
  modularity.cpp
  OutlineTables.cpp
  ProjectCtl.cpp
  ProjectCtlTables.cpp
  ProjectCtlWin.cpp
  ProjectMenu.cpp
  ProjectMgr.cpp
  ProjectMgrEditor.cpp
  ProjectMgrTables.cpp
  utils.cpp
  WorkSubject.cpp

  builtin_mods/ccpp/CodeEditorCCPP.cpp
  builtin_mods/ccpp/mod_ccpp.cpp
  builtin_mods/go/CodeEditorGo.cpp
  builtin_mods/go/mod_go.cpp
  # builtin_mods/python/CodeEditorPy.cpp
  # builtin_mods/python/mod_py.cpp
)

#target_compile_options(
#ccedit
#PUBLIC
#-Wall
#-Wextra
#-Wpedantic
#)

target_link_libraries(
  ccedit
  PUBLIC
  # -static
  PkgConfig::gtk4
  PkgConfig::glib
  PkgConfig::gtkmm4
  PkgConfig::sigc
  Boost::json
  Boost::regex
  ICU::uc
  ICU::i18n
)

target_include_directories(
  ccedit
  PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${ICU_INCLUDE_DIRS}
)
