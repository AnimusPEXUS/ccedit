# CCUCTS sercher/downloader. version 2026-07-21T14:45`

message(STATUS "==v== CCUCTS searcher/downloader ==v==")

set(CCUCTS_IMPORT_FILENAME "_import_me.cmake")

if (NOT DEFINED CCUCTS_VERSION)
  
  cmake_path(GET CMAKE_CURRENT_LIST_FILE PARENT_PATH CCUCTS_DIR_SEARCH_CWD)
  cmake_path(ABSOLUTE_PATH CCUCTS_DIR_SEARCH_CWD BASE_DIRECTORY / NORMALIZE)

  message(STATUS "CCUCTS isn't ready. searching for CCUCTS installation...")
  message(STATUS "checking CCUCTS_DIR variable")
  if ((DEFINED CCUCTS_DIR))
    message(STATUS "CCUCTS_DIR already defined. trying to import from ${CCUCTS_DIR}")
    if (EXISTS "${CCUCTS_DIR}/${CCUCTS_IMPORT_FILENAME}")
      message(STATUS "check success. importing CCUCTS...")
      include("${CCUCTS_DIR}/${CCUCTS_IMPORT_FILENAME}")
    endif()
  endif()

  if (NOT DEFINED CCUCTS_VERSION)
    message(STATUS "failed. trying to find CCUCTS in parent path")
    set(CCUCTS_DIR_SEARCH_CURRENT_PATH "${CCUCTS_DIR_SEARCH_CWD}")

    while(TRUE)
      cmake_path(GET CCUCTS_DIR_SEARCH_CURRENT_PATH PARENT_PATH CCUCTS_DIR_SEARCH_CURRENT_PATH)
      # message(STATUS "CCUCTS_DIR_SEARCH_CURRENT_PATH == ${CCUCTS_DIR_SEARCH_CURRENT_PATH}")
      if ((CCUCTS_DIR_SEARCH_CURRENT_PATH STREQUAL "") OR (CCUCTS_DIR_SEARCH_CURRENT_PATH STREQUAL "/"))
          break()
      endif()
      message(STATUS "searching in ${CCUCTS_DIR_SEARCH_CURRENT_PATH}/ccucts")
      set(CCUCTS_DIR_SEARCH_CURRENT_PATH_FILE_TO_CHECK "${CCUCTS_DIR_SEARCH_CURRENT_PATH}/ccucts/${CCUCTS_IMPORT_FILENAME}")
      if (EXISTS "${CCUCTS_DIR_SEARCH_CURRENT_PATH_FILE_TO_CHECK}")
        message(STATUS "importing file ${CCUCTS_DIR_SEARCH_CURRENT_PATH_FILE_TO_CHECK}")
        include("${CCUCTS_DIR_SEARCH_CURRENT_PATH_FILE_TO_CHECK}")
        if (DEFINED CCUCTS_VERSION)
          break()
        endif()
      endif()

    endwhile()
  endif()

  if (NOT DEFINED CCUCTS_VERSION)
    message(STATUS "failed to find existing CCUCTS. downloading...")
  
    include(FetchContent)
  
    FetchContent_Declare(
      ccucts_download
      GIT_REPOSITORY "https://github.com/AnimusPEXUS/ccucts.git"
      GIT_TAG "origin/master"
    )

    FetchContent_MakeAvailable(ccucts_download)

    FetchContent_GetProperties(
      ccucts_download
      SOURCE_DIR CCUCTS_DIR
    )

    if (EXISTS "${CCUCTS_DIR}/${CCUCTS_IMPORT_FILENAME}")
      message(STATUS "downloaded. importing CCUCTS...")
      include("${CCUCTS_DIR}/${CCUCTS_IMPORT_FILENAME}")
    else()
      message(STATUS "downloading failed.")
    endif()

  endif()

  if (NOT DEFINED CCUCTS_VERSION)
    message(FATAL_ERROR "CCUCTS unavailable. can't proseed")
  else()
    message(STATUS "CCUCTS deployed")
  endif()

else()
  message(STATUS "already defined CCUCTS version detected: ${CCUCTS_VERSION}")
endif()

unset(CCUCTS_DIR_SEARCH_CURRENT_PATH)
unset(CCUCTS_DIR_SEARCH_CURRENT_PATH_FILE_TO_CHECK)

message(STATUS "==^== CCUCTS searcher/downloader ==^==")
