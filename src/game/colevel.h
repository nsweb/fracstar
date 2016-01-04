


#ifndef FSCOLEVEL_H
#define FSCOLEVEL_H

#include "engine/component.h"
#include "gfx/shader.h"

namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};

class UniformVariable : public ShaderUniformDetail
{
public:
    int     m_buffer_offset;
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
    Name                    m_level_name;
    Shader*                 m_df_shader;
    Array<UniformVariable>  m_shader_variables;
    Array<uint8>            m_uniform_buffer;
    
    bool                LoadShader();
    void                CreateUniformVariables();
};

#endif // FSCOLEVEL_H