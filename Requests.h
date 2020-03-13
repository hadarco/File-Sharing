//Requests guard.
#ifndef REQUESTS_H 
#define REQUESTS_H

//Include dependencies. 
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fcgio.h"

//Defintion of the path directory to store the files.
#define FILEDIRECTORYPATH "/usr/share/nginx/"

//Definion buffer size.
#define MAX_SIZE  256

//Definiton of the file content buffer size.
#define BUF_SIZE 1000000

//Definion of none use flag.
#define NONE 0

//Defintion of equal chars arrays.
#define EQUALS 0

//Defintion of not exist directory.
#define NOTEXIST -1

//Defintion of the max size of the input buffer.
#define INPUTBUFFERMAXSIZE 1000000ul

//Defintion of the create user command.
#define CREATEUSERCOMMAND "create_user"

//Defintion of the download file command.
#define DOWNLOADFILECOMMAND "download_file"

//Defintion of the upload file command.
#define UPLOADFILECOMMAND "upload_file"

//Definiton of the permssion for the user.
#define USERADMINRIGHTS 0700

using namespace std;

/**
The function will retrive the file content from the requsest.
@param FCGXRequest The request.
@return string value that representing file content from the request.
*/
string GetContentRequest(const FCGX_Request & FCGXRequest);

/**
The function will extract the request, user name and file name from the request address.
@param RequestAddress The url address of the request.
@param Request buffer to store the extracted request.
@param UserName buffer to store the extracted user name.
@param FileName buffer to store the extracted file name.
@return boolean value that representing if the extract the request, user name and file name from the request address was success or not.
*/
bool ParseRequest(const char * RequestAddress, char Request[], char UserName[], char FileName[]);

/**
The function will process the request.
@param FCGXRequest The request.
@param ProcessOutputStreamBuffer steam buffer output of the console.
@return boolean value that representing if the request processed or not.
*/
bool ProcessRequest(FCGX_Request FCGXRequest, streambuf * ProcessOutputStreamBuffer);

#endif //REQUESTS


