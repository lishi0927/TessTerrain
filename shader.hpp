#ifndef SHADER_HPP
#define SHADER_HPP
#include "util.h"

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

GLuint LoadTessShaders(const char * vertex_file_path, const char * tessellation_control_path, const char* tessellation_evaluation_path, const char * fragment_file_path);

#endif
