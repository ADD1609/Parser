cmake_minimum_required(VERSION 3.20)
project(Parser)

set(CMAKE_CXX_STANDARD 23)

add_executable(Parser Tokenizer.cpp Tokenizer.h gauss_ext.cpp gauss_ext.h main.cpp Parser.cpp Parser.h Reader.cpp Reader.h User_funcs.cpp User_funcs.h)
