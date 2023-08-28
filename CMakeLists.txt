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

pkg_check_modules(gtkmm REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_executable(
    codeedit 
    main.cpp
    Controller.cpp
    ProjectMgr.cpp
    ProjectMgrEditor.cpp
    ProjectCtl.cpp
    FileExplorer.cpp
    FileExplorerWidget.cpp
    EditorListView.cpp
    WorkSubject.cpp
    WorkSubjectListView.cpp
    modularity.cpp
    module_ccpp.cpp
    )

target_link_libraries(
    codeedit 
    PUBLIC 
    # -static
    PkgConfig::gtkmm 
    Boost::json
    ${Boost_LIBRARIES}
    )

target_include_directories(
    codeedit 
    PUBLIC 
    ${Boost_INCLUDE_DIRS}
    )

