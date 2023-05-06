
idf_component_register(
	SRCS
		"src/abi.cpp"
		"src/runner.cpp"
		"src/runner.S"
		"src/progloader.cpp"
	INCLUDE_DIRS
		"src"
		"elfloader/src"
		"kernel/src"
	REQUIRES
		"esp_timer"
)

add_subdirectory(elfloader)
add_subdirectory(kernel)
target_link_libraries(${COMPONENT_LIB} PUBLIC elfloader kernel)
