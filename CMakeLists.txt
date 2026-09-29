# Asegurar compatibilidad con CMake 4.x
set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "Minimum policy version" FORCE)
cmake_minimum_required(VERSION 3.16)
project(computador_systemc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir encabezados del proyecto
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 1. Intentar encontrar SystemC instalado en el sistema
find_package(SystemCLanguage CONFIG QUIET)

if(SystemCLanguage_FOUND)
    message(STATUS "[OK] SystemC detectado en el sistema via CMake Config.")
    add_executable(adder_8bit_sim src/main.cpp)
    target_link_libraries(adder_8bit_sim PRIVATE SystemC::systemc)
else()
    # 2. Verificar si el usuario definio la variable de entorno SYSTEMC_HOME
    if(DEFINED ENV{SYSTEMC_HOME})
        message(STATUS "[OK] Usando SYSTEMC_HOME: $ENV{SYSTEMC_HOME}")
        include_directories($ENV{SYSTEMC_HOME}/include)
        link_directories(
            $ENV{SYSTEMC_HOME}/lib-mingw64
            $ENV{SYSTEMC_HOME}/lib-linux64
            $ENV{SYSTEMC_HOME}/lib
            $ENV{SYSTEMC_HOME}/lib64
        )
        add_executable(adder_8bit_sim src/main.cpp)
        target_link_libraries(adder_8bit_sim PRIVATE systemc pthread)
    else()
        # 3. Descarga y compilacion automatica de SystemC desde el repositorio oficial de Accellera
        message(STATUS "[INFO] SystemC no encontrado localmente. Descargando Accellera SystemC 3.0.2 automaticamente...")
        include(FetchContent)
        FetchContent_Declare(
            SystemC
            GIT_REPOSITORY https://github.com/accellera-official/systemc.git
            GIT_TAG        3.0.2
        )
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static SystemC" FORCE)
        set(ENABLE_ASSERTIONS ON CACHE BOOL "Enable SystemC assertions" FORCE)
        set(SystemC_ENABLE_TESTING OFF CACHE BOOL "Disable SystemC tests" FORCE)
        FetchContent_MakeAvailable(SystemC)

        add_executable(adder_8bit_sim src/main.cpp)
        target_link_libraries(adder_8bit_sim PRIVATE SystemC::systemc)
    endif()
endif()
