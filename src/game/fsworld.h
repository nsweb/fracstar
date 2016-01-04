


#ifndef FSWORLD_H
#define FSWORLD_H

#include "engine/componentmanager.h"

namespace bigball
{
	class BIGBALL_API Component;
	class BIGBALL_API Shader;
    struct BIGBALL_API RenderContext;
};
class CoLevel;

class FSWorld : public ComponentManager
{
    STATIC_MANAGER_H(FSWorld);
    
private:
    typedef ComponentManager Super;

public:
                            FSWorld();
	virtual                 ~FSWorld();

	virtual void            Create();
	virtual void            Destroy();
	virtual void            AddComponentToWorld( Component* pComponent );
	virtual void            RemoveComponentFromWorld( Component* pComponent );
	virtual void            Tick( struct TickContext& TickCtxt );
    virtual void            _Render( bigball::RenderContext& RenderCtxt );

    bool                    InitLevels( char const* json_path );
	Array<CoLevel*> const&  GetLevelArray()	{ return m_levels;	}
    CoLevel*                GetCurrentLevel();
    bool                    SetCurrentLevel( int level_idx );

protected:
	Array<CoLevel*>		m_levels;
    int                 m_current_level_idx;
};


#endif // FSWORLD_H