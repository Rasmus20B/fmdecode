set(SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/main.cc
)

add_library(fm)
target_sources(fm
  PUBLIC
  FILE_SET CXX_MODULES FILES
  block.cc
  chunk.cc
  decode.cc
  header.cc 
  sector.cc
  )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE fm)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
