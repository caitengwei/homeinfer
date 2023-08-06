cmake_minimum_required(VERSION 3.16)

project (homeinfer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Werror)


find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG QUIET)

if(gRPC_FOUND)
    message(STATUS "Found gRPC++: ${gRPC_VERSION}")
    set(GRPCPP_INCLUDE_DIRS $<TARGET_PROPERTY:gRPC::grpc++,INTERFACE_INCLUDE_DIRECTORIES>)
    set(GRPCPP_LIBRARIES $<TARGET_PROPERTY:gRPC::grpc++_unsecure,INTERFACE_LINK_LIBRARIES>)
    set(GRPCPP_PLUGIN_EXEC $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GRPCPP REQUIRED QUIET grpc++_unsecure)
    set(GRPCPP_PLUGIN_EXEC ${GRPCPP_PREFIX}/bin/grpc_cpp_plugin)
    message(STATUS "Found gRPC++: ${GRPCPP_LINK_LIBRARIES} (found version \"${GRPCPP_VERSION}\")")
endif()

set(SRC_DIR ${CMAKE_SOURCE_DIR})
set(PROTO_DIR ${CMAKE_SOURCE_DIR}/server/grpc/proto)
set(PROTO_GENERATE_DIR ${CMAKE_SOURCE_DIR}/server/grpc/generated)

# Proto file
get_filename_component(infer_proto "${PROTO_DIR}/inference.proto" ABSOLUTE)
get_filename_component(infer_proto_path "${infer_proto}" PATH)
get_filename_component(infer_proto_name "${infer_proto}" NAME_WE)
#generated sources
set(PROTO_SRCS "${PROTO_GENERATE_DIR}/${infer_proto_name}.pb.cc")
set(PROTO_HDRS "${PROTO_GENERATE_DIR}/${infer_proto_name}.pb.h")
set(GRPC_SRCS "${PROTO_GENERATE_DIR}/${infer_proto_name}.grpc.pb.cc")
set(GRPC_HDRS "${PROTO_GENERATE_DIR}/${infer_proto_name}.grpc.pb.h")

add_custom_command(
    OUTPUT "${PROTO_SRCS}" "${PROTO_HDRS}" "${GRPC_SRCS}" "${GRPC_HDRS}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${SRC_DIR}/server/grpc/generated"
        --cpp_out "${SRC_DIR}/server/grpc/generated"
        -I "${infer_proto_path}"
        --plugin=protoc-gen-grpc="${GRPCPP_PLUGIN_EXEC}"
        "${infer_proto}"
    DEPENDS "${infer_proto}"
    COMMENT "Executing protoc to generate grpc files")


include_directories(${GRPCPP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(InferenceServer
               ${SRC_DIR}/server/grpc/inference_server.cc
               ${PROTO_SRCS}
               ${GRPC_SRCS}
)
# target_include_directories(FileExchangeServer PRIVATE ${SRC_DIR}/server ${SRC_DIR}/common)
target_link_libraries(InferenceServer
                      ${GRPCPP_LIBRARIES}
                      ${Protobuf_LIBRARIES})