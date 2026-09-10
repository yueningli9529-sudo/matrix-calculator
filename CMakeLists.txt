cmake_minimum_required(VERSION 3.19)
project(MatrixCalculator LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(MatrixCalculator
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    core/Matrix.hpp
    core/Matrix.tpp
    core/MatrixOps.tpp
    calculation/MatrixStep.h
    core/Rational.hpp
    core/Rational.cpp
    dialog.h dialog.cpp dialog.ui
    MatrixType.h
    archive/MatrixRecord.h
    archive/MatrixRepository.h archive/MatrixRepository.cpp
    archive/MatrixJsonSerializer.h archive/MatrixJsonSerializer.cpp
    archive/MatrixFileStorage.h archive/MatrixFileStorage.cpp
    archive/MatrixSaveSystem.h archive/MatrixSaveSystem.cpp
    pages/inputpage.h pages/inputpage.cpp pages/inputpage.ui
    pages/calculatepage.h pages/calculatepage.cpp pages/calculatepage.ui
    widgets/matrix/matrixwidget.h widgets/matrix/matrixwidget.cpp widgets/matrix/matrixwidget.ui
    widgets/matrix/matrixtablemodel.h widgets/matrix/matrixtablemodel.cpp
    widgets/matrix/matrixitemdelegate.h widgets/matrix/matrixitemdelegate.cpp

)

target_include_directories(MatrixCalculator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/archive
)

target_link_libraries(MatrixCalculator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS MatrixCalculator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MatrixCalculator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
