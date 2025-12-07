cmake_minimum_required(VERSION 3.19)
project(SudokuSolver LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(SudokuSolver
    WIN32 MACOSX_BUNDLE
    main.cpp

    mainwindow.h
    mainwindow.ui
    resizer.tpp
    mainwindow.cpp
    libs/SudokuSolverAlgorithm.h
)

qt_add_translations(
    TARGETS SudokuSolver
    TS_FILES SudokuSolver_it_IT.ts
)

# Aggiungi libreria
add_library(libSudokuSolverAlgorithm SHARED IMPORTED)
set_target_properties(libSudokuSolverAlgorithm PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libSudokuSolverAlgorithm.dll
    IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/libs/libSudokuSolverAlgorithm.dll.a
)

target_link_libraries(SudokuSolver
    PRIVATE
        Qt::Core
        Qt::Widgets
        libSudokuSolverAlgorithm
)

include(GNUInstallDirs)

install(TARGETS SudokuSolver
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SudokuSolver
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Copia DLL (Windows)
if(WIN32)
    add_custom_command(TARGET SudokuSolver POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/libs/libSudokuSolverAlgorithm.dll
        $<TARGET_FILE_DIR:SudokuSolver>
    )
endif()
