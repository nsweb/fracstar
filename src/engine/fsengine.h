
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

	virtual bool		Init( bool bCreateWindow );
	virtual void		Shutdown();
	virtual void		MainLoop();
	virtual bool		RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens );

	static FSEngine*	Get()		{ return ms_pengine;	}

protected:
	virtual void		DeclareComponentsAndEntities();
	virtual void		CreateGameCameras();
	virtual void		InitManagers();
	virtual void		DestroyManagers();

	static FSEngine*	ms_pengine;
	class FSEditor*		m_peditor;
};


#endif  // MARSENGINE_H