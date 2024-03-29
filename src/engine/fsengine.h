
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
	virtual bool		MainLoop() override;
	virtual bool		RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens ) override;

	static FSEngine*	Get()		{ return static_cast<FSEngine*>(ms_engine);	}

protected:
	virtual void		DeclareComponentsAndEntities() override;
	virtual void		CreateGameCameras() override;
	virtual void		InitManagers() override;
	virtual void		DestroyManagers() override;

	class FSEditor*		m_editor;
};


#endif  // FSENGINE_H