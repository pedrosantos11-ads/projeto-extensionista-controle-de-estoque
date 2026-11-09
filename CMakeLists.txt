cmake_minimum_required(VERSION 3.16)
project(ProjetoExtensao LANGUAGES CXX)

include(FetchContent)

option(FTXUI_USE_SYSTEM_PACKAGE
       "Usar uma instalação do FTXUI fornecida pelo sistema"
       OFF)

if(FTXUI_USE_SYSTEM_PACKAGE)
    find_package(ftxui CONFIG REQUIRED)
else()
    FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG v7.0.3
    )
    FetchContent_MakeAvailable(ftxui)
endif()

add_executable(sistema
        codigo/main.cpp
        codigo/util.cpp
        codigo/ui.cpp
        codigo/telas_estoque.cpp
        codigo/telas_vendas.cpp
)

target_compile_features(sistema PRIVATE cxx_std_17)
set_target_properties(sistema PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sistema PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/codigo")
target_link_libraries(sistema PRIVATE ftxui::component ftxui::dom ftxui::screen)
