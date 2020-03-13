//Include dependencies. 
#include "Requests.h"

/**
The function will retrive the file content from the requsest.
@param FCGXRequest The request.
@return string value that representing file content from the request.
*/
string GetContentRequest(const FCGX_Request & FCGXRequest)
{
	//Get the content length of the request.
	char * ContentLengthString = FCGX_GetParam("CONTENT_LENGTH", FCGXRequest.envp);
	
	//Declartion of the variable type integer that will store the content length.
	unsigned long ContentLength = INPUTBUFFERMAXSIZE;

	//If there data in the content length of the request.
	if (ContentLengthString) 
	{
		//Convert the content length of the request to number.
		ContentLength = strtol(ContentLengthString, &ContentLengthString, 10);

		//If there is error then print to the user error massege.
		if (*ContentLengthString)
		{
			cerr << "Can't Parse CONTENT_LENGTH= '" << FCGX_GetParam("CONTENT_LENGTH", FCGXRequest.envp) << "'. Consuming stdin up to " << INPUTBUFFERMAXSIZE << endl;
		}

		//Set the size of the content length.
		if (ContentLength > INPUTBUFFERMAXSIZE)
		{
			ContentLength = INPUTBUFFERMAXSIZE;
		}
	}
	else 
	{
		//Reset the content length if CONTENT_LENGTH is missing.
		ContentLength = 0;
	}

	//Read the file content from the request.
	char * ContentBuffer = new char[ContentLength];
	cin.read(ContentBuffer, ContentLength);
	ContentLength = cin.gcount();

	//Remove the rest of the data from the buffer.
	do cin.ignore(1024); while (cin.gcount() == 1024);

	//Create string of the content.
	string content(ContentBuffer, ContentLength);

	delete[] ContentBuffer;
	
	//return the string content.
	return content;
}

/**
The function will extract the request, user name and file name from the request address.
@param RequestAddress The url address of the request.
@param Request buffer to store the extracted request.
@param UserName buffer to store the extracted user name.
@param FileName buffer to store the extracted file name.
@return boolean value that representing if the extract the request, user name and file name from the request address was success or not.
*/
bool ParseRequest(const char * RequestAddress, char Request[], char UserName[], char FileName[]) 
{
	int RequestAddressSize = strlen(RequestAddress);
	//If the request address is not valid then return failed status.
	if (RequestAddress[0] != '/')
	{
		return false;
	}

	//Declaration of index.
	int Index;

	int RequestAddressIndex = 1;

	//copy the request from the request address.
	for (Index = 0; (RequestAddress[RequestAddressIndex] != '/') && (Index < MAX_SIZE) && (RequestAddressIndex < RequestAddressSize); Index++, RequestAddressIndex++)
	{
		Request[Index] = RequestAddress[RequestAddressIndex];
	}

	RequestAddressIndex++;

	//copy the user name from the request address.
	for (Index = 0; (RequestAddress[RequestAddressIndex] != '/') && (Index < MAX_SIZE) && (RequestAddressIndex < RequestAddressSize); Index++, RequestAddressIndex++)
	{
		UserName[Index] = RequestAddress[RequestAddressIndex];
	}

	RequestAddressIndex++;

	//copy the file name from the request address.
	for (Index = 0; (RequestAddress[RequestAddressIndex] != '/') && (Index < MAX_SIZE) && (RequestAddressIndex < RequestAddressSize); Index++, RequestAddressIndex++)
	{
		FileName[Index] = RequestAddress[RequestAddressIndex];
	}

	return true;
}

/**
The function will process the request.
@param FCGXRequest The request.
@param ProcessOutputStreamBuffer steam buffer output of the console.
@return boolean value that representing if the request processed or not.
*/
bool ProcessRequest(FCGX_Request FCGXRequest, streambuf * ProcessOutputStreamBuffer)
{
	//Stream to the console output.
	ostream ConsoleOutput(ProcessOutputStreamBuffer);

	//Getting the address of the request.
	const char * RequestAddress = FCGX_GetParam("REQUEST_URI", FCGXRequest.envp);

	//print message to the console that strting to process the request.
	ConsoleOutput << "Starting to process the request." << endl;
	
	//Declaration and initialization of arrays of chars that will store the request, the username and the file name.
	char Request[MAX_SIZE] = { NONE };
	char UserName[MAX_SIZE] = { NONE };
	char FileName[MAX_SIZE] = { NONE };

	//print message to the console that strting to parsing the request.
	ConsoleOutput << "Start parsing the request" << endl;

	//Extract the request, user name and file name from the request address.
	if (ParseRequest(RequestAddress, Request, UserName, FileName) == false)
	{
		//if the extraction failed then print error message to the console, finish and return that the request could not processed.
		ConsoleOutput << "error parsing the request" << endl;

		return false;
	}

	//print message to the console that finish to parsing the request.
	ConsoleOutput << "Finish parsing the request" << endl;

	//Get the user directory in the server.
	string FilesDirectoryPath(string(FILEDIRECTORYPATH));
	FilesDirectoryPath.append(string(UserName));

	//Check if the request is to create user.
	if (strcmp(Request, CREATEUSERCOMMAND) == EQUALS)
	{
		//print message to the console of the request data.
		ConsoleOutput << "Request: create user" << endl << "UserName: " << UserName << endl;

		//print message to the console that starting to create user.
		ConsoleOutput << "Starting to create user" << endl;

		//Declartion of object of the type stat that will store the directory status.
		struct stat st = { NONE };

		//check if the directory don't exist. 
		if (stat(FilesDirectoryPath.c_str(), &st) == -1) 
		{
			//if the directory don't exist then print message to the console that creating direcroty for the user, create it and end the return the the process of the request was success. 
			ConsoleOutput << "Creating directory for the user " << UserName << endl;
			
			mkdir(FilesDirectoryPath.c_str(), USERADMINRIGHTS);
			
			return true;
		}
		else 
		{
			//if the directory exist then print message to the user and the console and return that the process could not been porcessed.
			ConsoleOutput << "Error the user " << UserName << " already exist." << endl;

			cout << "Error the user already exist." << endl;

			return false;
		}

	}

	//Check if the request is to download file.
	else if (strcmp(Request, DOWNLOADFILECOMMAND) == EQUALS && FileName != NULL)
	{
		//print message to the console of the request data.
		ConsoleOutput << "Request: download file" << endl << "UserName: " << UserName << endl << "FileName: " << FileName << endl;

		//print message to the console that starting to download the file.
		ConsoleOutput << "Statring the file download." << endl;

		//Declartion of object of the type stat that will store the directory status.
		struct stat st = { 0 };

		//Check if the user directory exist.
		if (stat(FilesDirectoryPath.c_str(), &st) != NOTEXIST)
		{
			//TO-DO check if file exist
			void * content;
			int bytes_to_write;
			FILE * pFile;

			//Get the user directory.
			FilesDirectoryPath.append(string("/"));
			FilesDirectoryPath.append(FileName);

			//print message to the console that entring user directory.
			ConsoleOutput << "Entring the user directory " << FilesDirectoryPath.c_str() << endl;

			//opening the requested file for read.
			pFile = fopen(FilesDirectoryPath.c_str(), "r");

			//print message to the console that opening the requested file.
			ConsoleOutput << "Opening the file " << FileName << endl;

			//if the file don't exist then print error message to the console and return that the process could not be processed.
			if (pFile == NULL) 
			{
				ConsoleOutput << "Error the file " << FileName << " don't exist." << endl;
				
				return false;
			}

			//print message to the console that allocating memory for the file.
			ConsoleOutput << "Allocating memory for the file " << FileName << endl;

			//Allocate memory for the file data.
			content = (void*)malloc(BUF_SIZE);

			//if the alloction failed then print message to the console and return that the request could not be processed.
			if (content == NULL)
			{
				ConsoleOutput << "Error allocating memory for the file " << FileName <<  " failed." << endl;

				return false;
			}

			//ReD the file content.
			while ((bytes_to_write = fread(content, 1, BUF_SIZE, pFile))) 
			{
				//Send the file content.
				cout << "\r\n" << (char*)content;
			}

			//If there error with the stdin then print message to the console and return that the request could not be processed.
			if (ferror(stdin))
			{
				ConsoleOutput << "error can't read from stdin." << endl;
			
				return false;
			}

			//Free the allocated memory/
			free(content);

			//Close the file.
			fclose(pFile);

			//print message to the console that finished to download the file.
			ConsoleOutput << "Finish the file download." << endl;

			//Finish the function and return that the request has been processed.
			return true;
		}
		else
		{
			//print to the user and the console that the file not exist and return that the request could not be processed.
			cout << "File not exist!" << endl;

			ConsoleOutput << "File not exist!" << endl;
			return false;
		}
	}

	//Check if the request is to upload file.
	else if (strcmp(Request, UPLOADFILECOMMAND) == EQUALS && FileName != NULL)
	{
		//print message to the console of the request data.
		ConsoleOutput << "Request: upload file" << endl << "UserName: " << UserName << endl << "FileName: " << FileName << endl;

		//print message to the console that starting to upload the file.
		ConsoleOutput << "Statring the file upload." << endl;

		//Get the content of the request.
		string content = GetContentRequest(FCGXRequest);
		printf("-----------------------------------------------------------Hi you gonna get the content of file-----------------------------------------------------------\n");
		printf("%s\n", content.c_str());
printf("-----------------------------------------------------------Hi you got the content of file-----------------------------------------------------------\n");
		FILE * pFile;

		//Declartion of object of the type stat that will store the directory status.
		struct stat st = { 0 };

		//Check if the user directory exist.
		if (stat(FilesDirectoryPath.c_str(), &st) != NOTEXIST)
		{
			//Get the file path.
			FilesDirectoryPath.append(string("/"));
			FilesDirectoryPath.append(FileName);

			ConsoleOutput << "Starting to write the file " << FileName <<  " to " << FilesDirectoryPath.c_str() << endl;
			
			//Open output stream to the file.
			ofstream out(FilesDirectoryPath.c_str());

			//Send to the user the file data.
			out << content.c_str();

			out.close();
			
			//print message to the console that finish to upload the file.
			ConsoleOutput << "finish the file upload." << endl;
		}
		else
		{
			//Print to the user error of user not exist and return that the request could not be process.
			cout << "Error user not exist." << endl;
			return false;
		}
	}
	else
	{
		//Print to the user and the console error of illegal request and return that the request could not be process.
		cout << "Error illegal request." << endl;

		ConsoleOutput << "Error illegal request." << endl;
		return false;
	}

	//Finish the function and return and that the request been processed.
	return true;
}


