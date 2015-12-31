


#ifndef FSCOLEVEL_H
#define FSCOLEVEL_H

#include "engine/component.h"


namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};


class CoLevel : public Component 
{
	CLASS_EQUIP_H(CoLevel, Component)

public:
						CoLevel();
	virtual				~CoLevel();

	static Component*	NewComponent()		{ return new CoLevel();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual void		Tick( TickContext& tick_ctxt );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );

public:
    Name            m_level_name;
    Shader*			m_df_shader;
    
    bool                LoadShader();
};

#endif // FSCOLEVEL_H