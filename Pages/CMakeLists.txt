project(Pages)

add_library(${PROJECT_NAME}
    UserStocksPage.h 
    UserStocksPage.cpp
    ExchangeGlassPage.h
    ExchangeGlassPage.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

include_directories(${Qt6_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Qt6::Widgets 
        Qt6::Gui   
        Qt6::Core 
        Qt6::Network
    PUBLIC 
        Models
)