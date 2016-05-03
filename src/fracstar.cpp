// fracstar.cpp : Defines the entry point for the console application.
//

#include "fracstar.h"
#include "system/profiler.h"
#include "SDL_main.h"

//#define KSVD_IMPLEMENTATION
//#define KSVD_NO_MATH
//#define KSVD_NO_IOSTREAM
//#define KSVD_USE_DOUBLE
//#include "ksvd/trunk/ksvd.h"

//int _tmain(int argc, _TCHAR* argv[])

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[])
{
	PROFILE_THREAD_SCOPE("MainThread");

	g_pEngine = new FSEngine();
	CommandLine& CmdLine = g_pEngine->GetCommandLine();
	CmdLine.Parse( argc, argv );
    
    EngineInitParams init_params;
    init_params.resizable_window = false;
    init_params.mouse_capture = true;
    init_params.default_res_x = 800;
    init_params.default_res_y = 600;
	g_pEngine->Init( init_params );

	String CmdType;
	if( CmdLine.IsCommand( CmdType ) )
	{
		g_pEngine->RunCommand( CmdType, CmdLine.switches, CmdLine.tokens );
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
