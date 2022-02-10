
#include "fracstar.h"
#include "common.h"

namespace
{
class FractstarApp : public entry::AppI
{
public:
	FractstarApp(const char* _name, const char* _description, const char* _url)
		: entry::AppI(_name, _description, _url)
	{
	}

	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		FSEngine* engine = new FSEngine();
		// (REBIND)
		//CommandLine& CmdLine = g_pEngine->GetCommandLine();
		//CmdLine.Parse(_argc, _argv);

		EngineInitParams init_params;
		init_params.resizable_window = false;
		init_params.mouse_capture = true;
		init_params.default_res_x = 1280;
		init_params.default_res_y = 720;
		init_params.argc = _argc;
		init_params.argv = _argv;
		init_params.app = this;
		FSEngine::Get()->Init(init_params);

		// (REBIND)
		//String CmdType;
		//if (CmdLine.IsCommand(CmdType))
		//{
		//	g_pEngine->RunCommand(CmdType, CmdLine.switches, CmdLine.tokens);
		//}
		//else
		//{
		//	g_pEngine->MainLoop();
		//}
	}

	int shutdown() override
	{
		FSEngine::Get()->Shutdown();
		delete FSEngine::Get();

		return 0;
	}

	bool update() override
	{
		return FSEngine::Get()->MainLoop();
	}
};

} // namespace

ENTRY_IMPLEMENT_MAIN(
	FractstarApp
	, "_fracstar_"
	, "Fracstar game"
	, "https://github.com/nsweb/fracstar"
	);
