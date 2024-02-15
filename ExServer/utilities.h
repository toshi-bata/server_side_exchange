#pragma once

void GetEnvironmentVariablePath(const char *varName, char *value, const bool isError);
void copy_file(const char *oDstFilePath, const char *oSrcFilePath);
long get_file_size(const char *filename);
char* load_file(const char *filename);
void getLowerExtention(const char *filename, char *lowext, char *filetype);
void getBaseName(const char* filename, char* basename);
void delete_files(char *dir);
#ifndef _WIN32
void delete_files(char* dir);
void delete_dirs(char *dir);
#else
void delete_files(const wchar_t* dir);
void delete_dirs(const wchar_t* dir);
#endif
