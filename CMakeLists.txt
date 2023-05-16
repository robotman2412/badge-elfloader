
if (NOT DEFINED ENV{BADGESDK_PATH})
	set(ENV{BADGESDK_PATH} $ENV{HOME}/.badgeteam/badgesdk)
endif()

idf_component_register(
	SRCS
		"src/abi.cpp"
		"src/abi/gpio.cpp"
		"src/abi/libc.cpp"
		"src/abi/system.cpp"
		"src/abi/math.cpp"
		"src/abi/softfloat.cpp"
		"src/abi/display.cpp"
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
target_include_directories(${COMPONENT_LIB} PUBLIC $ENV{BADGESDK_PATH}/..)
target_link_libraries(${COMPONENT_LIB} PUBLIC elfloader kernel)
