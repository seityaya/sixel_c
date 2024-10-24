cmake_minimum_required(VERSION 3.5)

project(sixel_c LANGUAGES C)

add_executable(
    ${PROJECT_NAME}
    main.c
    image.h image.c
    sixel.h sixel.c
    test_1.c
    test_3.c
    test_3.h
    mtime.h mtime.c
    test_2.c
    )
