macro(add_ngf_example NAME)
    add_executable("${NAME}" "${NAME}.cpp")
    target_link_libraries("${NAME}" ngf)
endmacro()

add_ngf_example(01_window)
add_ngf_example(02_inputs)
