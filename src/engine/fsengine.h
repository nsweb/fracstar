
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
	virtual bool		RunCommand( String const& CmdType, Array<String> const& Switches, Array<String> const& Tokens );

	static FSEngine*	Get()		{ return ms_pEngine;	}	

protected:
	virtual void		DeclareComponentsAndEntities();
	virtual void		CreateGameCameras();
	virtual void		InitManagers();
	virtual void		DestroyManagers();

	static FSEngine*	ms_pEngine;
	class FSEditor*		m_pEditor;
};


#endif  // MARSENGINE_H