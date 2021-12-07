project(abcg_horizon)
add_executable(${PROJECT_NAME} camera.cpp kart.cpp main.cpp labirinto.cpp openglwindow.cpp testarossa.cpp)
enable_abcg(${PROJECT_NAME})
