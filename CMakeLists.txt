
idf_component_register(
	SRCS
		"src/abi.cpp"
		"src/abi/gpio.cpp"
		"src/abi/libc.cpp"
		"src/abi/system.cpp"
		"src/runner.cpp"
		"src/runner.S"
		"src/progloader.cpp"
	INCLUDE_DIRS
		"src"
		"elfloader/src"
		"kernel/src"
	REQUIRES
		"esp_timer"
		"driver"
)

add_subdirectory(elfloader)
add_subdirectory(kernel)
target_link_libraries(${COMPONENT_LIB} PUBLIC elfloader kernel)
