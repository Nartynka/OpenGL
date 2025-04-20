cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

# suppress generating ZERO_CHECK project
set(CMAKE_SUPPRESS_REGENERATION true)

project(OpenGL)

find_package(OpenGL REQUIRED)

set(GLFW_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/GLFW/include")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/GLFW/lib-vc2022/glfw3.lib")

set(GLEW_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/GLEW/include")
set(GLEW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/GLEW/lib/Release/x64/glew32s.lib")

set(GLM_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib/glm")

set(STB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib")

set(HEADER_FILES
	
)

set(SOURCE_FILES
	"${CMAKE_SOURCE_DIR}/src/Application.cpp"
)

include_directories(
	${GLFW_INCLUDE_PATH}
	${GLEW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH}
	${STB_INCLUDE_PATH}
)

add_executable(${PROJECT_NAME}
	${HEADER_FILES}
	${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME}
	${OPENGL_LIBRARY}
	${GLFW_LIBRARY}
	${GLEW_LIBRARY}
	${STB_LIBRARY}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_compile_definitions(GLEW_STATIC)