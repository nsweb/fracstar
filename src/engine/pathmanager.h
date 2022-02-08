


#ifndef FSPATHMANAGER_H
#define FSPATHMANAGER_H

#include "engine/componentmanager.h"

namespace bigfx
{
	class BIGFX_API Component;
	class BIGFX_API Shader;
    struct BIGFX_API RenderContext;
};
class CoPath;

class PathManager : public ComponentManager 
{
	STATIC_MANAGER_H(PathManager);

private:
	typedef ComponentManager Super;

public:
						PathManager();
	virtual				~PathManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		AddComponentToWorld( Component* pComponent );
	virtual void		RemoveComponentFromWorld( Component* pComponent );
	virtual void		Tick( struct TickContext& TickCtxt );
    virtual void		_Render( bigfx::RenderContext& RenderCtxt );

	Array<CoPath*> const& GetPathArray()	{ return m_paths;	}
	//CoPath*				_GetCurrentPath()	{ if( m_paths.size() > 0 ) { return m_paths[0]; } return nullptr; }

protected:
	Array<CoPath*>		m_paths;
};


#endif // FSPATHMANAGER_H