cmake_minimum_required(VERSION 3.25.0)

project(Client)

set(CMAKE_CXX_STANDARD 17 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    main.cpp
    MainWindow.h
    MainWindow.cpp
    Context.h 
    Context.cpp
)

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
qt_standard_project_setup()
qt_add_executable(${PROJECT_NAME} "${SOURCES}")

add_subdirectory(Models)
add_subdirectory(Dialoges)
add_subdirectory(Requests)

include_directories(${Qt6_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Qt6::Widgets 
        Qt6::Gui   
        Qt6::Core 
        Qt6::Network
    PUBLIC 
        Models
        Dialoges
        Requests
)