//Include dependencies. 
#include "main.h"

/**
	The program.
	@param argc the number of recived arguments.
	@param argv array of recived arguments.
	@return integer value that representing if the run of the process was success or not.
*/
int main(int argc, char* argv[])
{
	//Backuping the input (cin), output (cout) and error (cerr) streams of the process.
	streambuf * ProcessInputStreamBuffer = cin.rdbuf();
	streambuf * ProcessOutputStreamBuffer = cout.rdbuf();
	streambuf * ProcessErrorStreamBuffer = cerr.rdbuf();

	//Stream to the console output.
	ostream ConsoleOutput(ProcessOutputStreamBuffer);

	//Declaration of variable type FCGX_Request that will represent the request object that will be shared accross the requests.
	FCGX_Request FCGXRequest;

	//Initializing the FCGX library.
	FCGX_Init();

	//Initializing the FCGX_Request object to use with FCGX_Accept_r().
	FCGX_InitRequest(&FCGXRequest, NONE, NONE);

	//Loop as long as the process can get new requests.
	while (FCGX_Accept_r(&FCGXRequest) == NONE)
	{
		//Printing message to the console that request as been recived.
		ConsoleOutput << "Request recived." << endl;

		//Getting the input, output and error streams of the request.
		fcgi_streambuf FCGIInputStreamBuffer(FCGXRequest.in);
		fcgi_streambuf FCGIOutputStreamBuffer(FCGXRequest.out);
		fcgi_streambuf FCGIErrorStreamBuffer(FCGXRequest.err);


		//Set the input, output and error streams of the request as the streams of the process.
		cin.rdbuf(&FCGIInputStreamBuffer);
		cout.rdbuf(&FCGIOutputStreamBuffer);
		cerr.rdbuf(&FCGIErrorStreamBuffer);

		//Printing message to the console that the request is been processed.
		ConsoleOutput << "Processing the request." << endl;

		//Processing the request.
		if (ProcessRequest(FCGXRequest, ProcessOutputStreamBuffer) == false)
		{
			//if there was error in the request processing then print error message to the request.
			cout << "Error proceesing the request." << endl;
		}
		//Print message to the console that the process has been processed.
		ConsoleOutput << "Finished processing the request." << endl;
		
		
		cout << "";
	}

	//Restore the input, output and error streams of the process. 
	cin.rdbuf(ProcessInputStreamBuffer);
	cout.rdbuf(ProcessOutputStreamBuffer);
	cerr.rdbuf(ProcessErrorStreamBuffer);

	//finish the program and return that the run was success.
	return SUCCESSSTATUS;
}
