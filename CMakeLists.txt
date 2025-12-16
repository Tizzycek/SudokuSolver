cmake_minimum_required(VERSION 3.19)
project(SudokuSolver LANGUAGES CXX)

# Integrazione per CLion
# Abilita le funzionalità automatiche di Qt
# CMAKE_AUTOMOC ON: Abilita il Meta-Object Compiler (MOC)
# CMAKE_AUTOUIC ON: Abilita lo User Interface Compiler (UIC) per i file .ui
# CMAKE_AUTORCC ON: Abilita il Resource Compiler (RCC) per i file .qrc
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON) # Aggiunge la directory corrente ai percorsi di include

# Specifica il percorso della configurazione di Qt
# DEVI cambiare il percorso in base alla tua installazione di Qt!
#set(CMAKE_PREFIX_PATH "F:/Qt/6.10.0/mingw_64/lib/cmake")

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

)

qt_add_translations(
    TARGETS SudokuSolver
    TS_FILES translations/SudokuSolver_it_IT.ts
)

# Aggiungi libreria
#add_library(libSudokuSolverAlgorithm SHARED IMPORTED)
#set_target_properties(libSudokuSolverAlgorithm PROPERTIES
#    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libSudokuSolverAlgorithm.dll
#    IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/libs/libSudokuSolverAlgorithm.dll.a
#)
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

# Copia DLL (Windows)
if(WIN32)
    add_custom_command(TARGET SudokuSolver POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/libs/libSudokuSolverAlgorithm.dll
        $<TARGET_FILE_DIR:SudokuSolver>
    )
endif()
