cmake_minimum_required(VERSION 3.19)
project(SudokuSolver LANGUAGES CXX)

# Specifica il percorso della configurazione di Qt
# DEVI cambiare il percorso in base alla tua installazione di Qt!
# set(CMAKE_PREFIX_PATH "F:/Qt/6.10.0/mingw_64/lib/cmake")
# in cmake options: -DCMAKE_PREFIX_PATH="F:/Qt/6.10.0/mingw_64/lib/cmake"

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(SudokuSolver
    WIN32 MACOSX_BUNDLE
    main.cpp

    mainwindow.h
    mainwindow.ui
    resizer.tpp
    mainwindow.cpp

    solverworker.h solverworker.cpp
        StartupDialog.cpp
        StartupDialog.h
        AppManager.cpp
        AppManager.h
        SingleIstance.cpp
        SingleIstance.h

)

qt_add_translations(
    TARGETS SudokuSolver
    TS_FILES translations/SudokuSolver_it_IT.ts translations/SudokuSolver_en_GB.ts translations/SudokuSolver_de_DE.ts
)

add_library(libSudokuSolverAlgorithm SHARED ${CMAKE_SOURCE_DIR}/libs/SudokuSolverAlgorithm.cpp
    libs/SudokuSolverAlgorithm.h)

target_link_libraries(SudokuSolver
    PRIVATE
        Qt::Core
        Qt::Widgets
        libSudokuSolverAlgorithm
)
target_link_libraries(SudokuSolver PRIVATE Qt6::Core)

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

# Copia DLL (Windows) nella cartella dell'eseguibile dopo la compilazione
if(WIN32)
    add_custom_command(TARGET SudokuSolver POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:libSudokuSolverAlgorithm>
            $<TARGET_FILE_DIR:SudokuSolver>
    )
endif()