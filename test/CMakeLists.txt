find_package(Catch2 REQUIRED)

add_executable(tests
	main.cpp
    RegistryImplTests.cpp )
target_link_libraries(tests
    Catch2::Catch2
    ctrail
)

add_test(NAME tests COMMAND tests)
