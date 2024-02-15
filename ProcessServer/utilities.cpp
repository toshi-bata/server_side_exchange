#include "utilities.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#ifndef _WIN32
#include <dirent.h>
#include<unistd.h>
#else
#include <Windows.h>
#include <direct.h>
#endif

void GetEnvironmentVariablePath (const char *varName, char *value, const bool isError)
{
	char *path = getenv(varName);
	if (!path)
	{
		if (isError)
			printf("Error: Environment variable: %s not found.\n", varName);
		else
			printf("Environment variable: %s not found.\n", varName);
		return;
	}

	strcat(value, path);

	int lastId = strlen(path) - 1;
	
#ifdef _MSC_VER
	if ('\\' != path[lastId])
		strcat(value, "\\");
#else
	if ('/' != path[lastId])
		strcat(value, "/");
#endif
}

void copy_file(const char *oDstFilePath, const char *oSrcFilePath)
{
	std::ifstream ifstr(oDstFilePath, std::ios::binary);
	std::ofstream ofstr(oSrcFilePath, std::ios::binary);

	ofstr << ifstr.rdbuf();
}

long get_file_size(const char *filename)
{
	FILE *fp;

	fp = fopen(filename, "rb");
	if (fp)
	{
		long size;

		if ((0 != fseek(fp, 0, SEEK_END)) || (-1 == (size = ftell(fp))))
			size = 0;

		fclose(fp);

		return size;
	}
	else
		return 0;
}

char* load_file(const char *filename)
{
	FILE *fp;
	char *buffer;
	long size;

	size = get_file_size(filename);
	if (0 == size)
		return NULL;

	fp = fopen(filename, "rb");
	if (!fp)
		return NULL;

	buffer = (char*)malloc(size + 1);
	if (!buffer)
	{
		fclose(fp);
		return NULL;
	}
	buffer[size] = '\0';

	if (size != (long)fread(buffer, 1, size, fp))
	{
		free(buffer);
		buffer = NULL;
	}

	fclose(fp);
	return buffer;
}

bool is_char_num(const char *str)
{
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] < '0'|| str[i] > '9')
			return false;
		i++;
	}

	return true;
}

char lower(char c) {
	if ('A' <= c && c <= 'Z') {
		c = c + ('a' - 'A');
	}
	return c;
}

void lowerstring(const char *in, char *out)
{
	int i = 0;
	while (in[i] != '\0') {
		out[i] = lower(in[i]);
		i++;
	}
	out[i] = '\0';
}

void get_creo_ext(const char *filename, size_t numlen, char *creoext, char *filetype)
{
	const char *exExts[] = { ".prt.", ".asm.", ".neu." };

	char lowFlename[FILENAME_MAX];
	lowerstring(filename, lowFlename);

	for (const char* ext : exExts)
	{
		const char *dotExt = strstr(filename, ext);
		if (NULL != dotExt)
		{
			size_t len = strlen(dotExt);
			if (len == strlen(ext) + numlen)
			{
				strcpy(creoext, (char*)&dotExt[1]);

				size_t typeLen = strlen(ext) - 2;
				for (int i = 0; i < typeLen; i++)
				{
					filetype[i] = ext[i + 1];
				}
				filetype[typeLen] = '\0';

				return;
			}

		}
	}
}

void getLowerExtention(const char *filename, char *lowext, char *filetype) {
	const char *dotExt = strrchr(filename, '.');

	if (NULL == dotExt)
		return;

	char ext[256];
	strcpy(ext, (char*)&dotExt[1]);

	if (is_char_num(ext))
	{
		// Pro/E, Creo
		char creoExt[256] = { '\0' };
		get_creo_ext(filename, strlen(ext), creoExt, filetype);

		if (strlen(creoExt))
			lowerstring(creoExt, lowext);
		
	}
	else
	{
		lowerstring(ext, lowext);
		strcpy(filetype, lowext);
	}
}

void getBaseName(const char* filename, char* basename)
{
	char lowext[256] = { "\0" };
	char filetype[256] = { "\0" };
	getLowerExtention(filename, lowext, filetype);

	int extlen = strlen(lowext);
	extlen++;

	int filelen = strlen(filename);

	filelen -= extlen;

	for (int i = 0; i < filelen; i++)
	{
		basename[i] = filename[i];
	}

	basename[filelen] = '\0';

}

#ifndef _WIN32
void delete_files(char *dir)
{
	DIR *dp;
    struct dirent *ent;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) {
        perror(dir);
        return;
    }
    chdir(dir);
    while ((ent = readdir(dp)) != NULL) {
        lstat(ent->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", ent->d_name) == 0 ||
                strcmp("..", ent->d_name) == 0)
                continue;
            delete_files(ent->d_name);
        }
        else {
            unlink(ent->d_name);
        }
    }
    chdir("..");
    closedir(dp);
}

void delete_dirs(char *dir)
{
	DIR *dp;
    struct dirent *ent;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) {
        perror(dir);
        return;
    }
    chdir(dir);
    while ((ent = readdir(dp)) != NULL) {
        lstat(ent->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", ent->d_name) == 0 ||
                strcmp("..", ent->d_name) == 0)
                continue;
            if (rmdir(ent->d_name) < 0) {
                delete_dirs(ent->d_name);
            }
        }
    }
    chdir("..");
    closedir(dp);

    if (rmdir(dir) < 0) {
        perror(dir);
        return;
    }

}

#else
void delete_files(const wchar_t* dir)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;

	wchar_t dir_wildpath[1024];
	wcscpy(dir_wildpath, dir);
	wcscat(dir_wildpath, L"*");

	hFind = FindFirstFile(dir_wildpath, &win32fd);
	if (hFind == INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		return;
	}

	do {
		if ((wcscmp(win32fd.cFileName, L".") == 0) || (wcscmp(win32fd.cFileName, L"..") == 0))
		{
		}
		else if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wchar_t dir_buf[1024];
			wcscpy(dir_buf, dir);
			wcscat(dir_buf, win32fd.cFileName);
			wcscat(dir_buf, L"\\");

			delete_files(dir_buf);
		}
		else
		{
			wchar_t full_path[1024];
			wcscpy(full_path, dir);
			wcscat(full_path, win32fd.cFileName);
			_wremove(full_path);
		}

	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);
}

void delete_dirs(const wchar_t* dir)
{
	wchar_t dir_copy[1024];
	wcscpy(dir_copy, dir);
	int lastId = wcslen(dir_copy) - 1;
	if ('\\' != dir_copy[lastId])
		wcscat(dir_copy, L"\\");

	delete_files(dir_copy);

	HANDLE hFind;
	WIN32_FIND_DATA win32fd;

	wchar_t dirwildpath[1024];

	wcscpy(dirwildpath, dir_copy);
	wcscat(dirwildpath, L"*");

	hFind = FindFirstFile(dirwildpath, &win32fd);
	if (hFind == INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		return;
	}

	do {
		if ((wcscmp(win32fd.cFileName, L".") == 0) || (wcscmp(win32fd.cFileName, L"..") == 0))
		{
		}
		else if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wchar_t dir_buf[1024];
			wcscpy(dir_buf, dir_copy);
			wcscat(dir_buf, win32fd.cFileName);
			
			if (0 != _wrmdir(dir_buf))
			{
				wcscat(dir_buf, L"\\");
				delete_dirs(dir_buf);
			}
		}

	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	_wrmdir(dir);
}
#endif
