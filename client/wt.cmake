Project(OnfidoTestProject)

SET(bin_src src/ClientUi.cpp
            src/main.cpp
            common/src/TcpClient.cpp
            common/src/TcpServer.cpp)
            #common/src/JsonFileParser.cpp)

SET(bin_name OnfidoClient)
SET(rapid_json /opt/rapidjson/include)
SET(wt_inc /usr/include/Wt)

FIND_PACKAGE(Threads REQUIRED)
ADD_EXECUTABLE(${bin_name} ${bin_src}) 
TARGET_LINK_LIBRARIES(${bin_name} pthread) 
TARGET_COMPILE_FEATURES(${bin_name} PUBLIC  cxx_lambda_init_captures)
TARGET_INCLUDE_DIRECTORIES(${bin_name} PUBLIC inc common/inc ${rapid_json} ${wt_inc})

TARGET_LINK_LIBRARIES(${bin_name}
  debug wtd      optimized wt
  debug wthttpd  optimized wthttp)
