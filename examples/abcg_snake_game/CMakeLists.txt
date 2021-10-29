project(abcg_snake_game)
add_executable(${PROJECT_NAME}  main.cpp 
                                openglwindow.cpp 
                                cobrinha.cpp
                                tabuleiro.cpp)
enable_abcg(${PROJECT_NAME})