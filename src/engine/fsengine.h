
// fsengine.h


#ifndef FSENGINE_H
#define FSENGINE_H

#include "engine/engine.h"

class FSEngine : public Engine
{
private:
	typedef Engine Super;

public:
						FSEngine();
	virtual				~FSEngine();

	virtual bool		Init(EngineInitParams const& init_params) override;
	virtual void		Shutdown() override;
	virtual void		MainLoop() override;
	virtual bool		RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens ) override;

	static FSEngine*	Get()		{ return ms_pengine;	}

protected:
	virtual void		DeclareComponentsAndEntities() override;
	virtual void		CreateGameCameras() override;
	virtual void		InitManagers() override;
	virtual void		DestroyManagers() override;

	static FSEngine*	ms_pengine;
	class FSEditor*		m_peditor;
};


#endif  // FSENGINE_H