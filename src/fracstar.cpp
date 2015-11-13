// fracstar.cpp : Defines the entry point for the console application.
//

#include "fracstar.h"
#include "system/profiler.h"

//#define KSVD_IMPLEMENTATION
//#define KSVD_NO_MATH
//#define KSVD_NO_IOSTREAM
//#define KSVD_USE_DOUBLE
//#include "ksvd/trunk/ksvd.h"

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{
	PROFILE_THREAD_SCOPE("MainThread");

	g_pEngine = new FSEngine();
	CommandLine& CmdLine = g_pEngine->GetCommandLine();
	CmdLine.Parse( argc, argv );
	g_pEngine->Init( true );

	String CmdType;
	if( CmdLine.IsCommand( CmdType ) )
	{
		g_pEngine->RunCommand( CmdType, CmdLine.Switches, CmdLine.Tokens );
	}
	else
	{
		g_pEngine->MainLoop();
	}

	g_pEngine->Shutdown();
	delete g_pEngine;
	g_pEngine = nullptr;

	return 0;
}
