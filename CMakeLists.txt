cmake_minimum_required(VERSION 3.5)

project(sixel LANGUAGES C)

add_executable(sixel
    main.c
    image.h image.c
    sixel.h sixel.c
    test_1.c
    test_2.c
    )
