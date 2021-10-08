cmake_minimum_required(VERSION 3.10)
project(Parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Parser Tokenizer.cpp Tokenizer.h gauss_ext.cpp gauss_ext.h main.cpp Parser.cpp Parser.h Reader.cpp Reader.h User_funcs.cpp User_funcs.h)
