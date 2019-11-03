find_package(Catch2 REQUIRED)

file(GLOB tests_src
    "*Tests.cpp"
)

add_executable(tests
	main.cpp
    ${tests_src}
)

target_link_libraries(tests
    Catch2::Catch2
    ctrail
)

add_test(NAME tests COMMAND tests)

