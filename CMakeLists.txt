project(b_server)
set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions("-Wall -pg -g")
include_directories(${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include 
${PROJECT_SOURCE_DIR}/httpclient 
${PROJECT_SOURCE_DIR}/../common ${PROJECT_SOURCE_DIR}/../cache_group 
${PROJECT_SOURCE_DIR}/../cache_group_new 
${PROJECT_SOURCE_DIR}/../zookeeper/include /usr/include/python2.6/
/home/liuhz/mongo-client-install/include/
${PROJECT_SOURCE_DIR}/../mongo_client/)
link_directories( ${PROJECT_SOURCE_DIR}/../common/lib
${PROJECT_SOURCE_DIR}/../cache_group/lib
${PROJECT_SOURCE_DIR}/../cache_group_new/lib
${PROJECT_SOURCE_DIR}/../zookeeper/lib 
${PROJECT_SOURCE_DIR}/../mongo_client/lib
/usr/local/lib /usr/lib64/ /home/liuhz/mongo-client-install/lib)
link_libraries(protobuf pthread boost_system boost_thread boost_filesystem boost_regex
log4cplus hiredis json uuid curl cachegroup cachegroupnew efnfw python2.6
libzookeeper_mt.a libmongo_client.a libmongoclient.a)
file(GLOB_RECURSE SRC_LIST src/*.cpp main.cpp proto/*.cc httpclient/*.cpp
embeded_python/*.cpp )
add_executable(bin/b_server ${SRC_LIST})