project(Dialoges)

add_library(${PROJECT_NAME}
    LoginWindow.h 
    LoginWindow.cpp
    RegisterWindow.h 
    RegisterWindow.cpp
    SellStocksWindow.h 
    SellStocksWindow.cpp
    BuyStocksWindow.h 
    BuyStocksWindow.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

include_directories(${Qt6_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Qt6::Widgets 
        Qt6::Gui   
        Qt6::Core 
        Qt6::Network
)