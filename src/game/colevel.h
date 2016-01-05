


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

class LevelVariableTrackBase
{
public:
    int        m_var_index;
    
    virtual int GetVarSize() const = 0;
    virtual void GetShaderUniformValue( Shader* shader, ShaderUniform const& uni, void* var_buffer ) = 0;
};

template <typename VarType>
class LevelVariableTrack : public LevelVariableTrackBase
{
public:
    struct Key
    {
        VarType     m_value;
        float       m_time;
    };

    Array<Key>  m_keys;
    
    virtual int GetVarSize() const   { return sizeof(VarType);   }
    virtual void GetShaderUniformValue( Shader* shader, ShaderUniform const& uni, void* var_buffer )
    {
        shader->GetUniform( uni, *(VarType*)var_buffer );
    }
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
    Name                            m_level_name;
    Shader*                         m_df_shader;
    Array<UniformVariable>          m_shader_variables;
    Array<uint8>                    m_uniform_buffer;
    Array<LevelVariableTrackBase*>  m_var_tracks;
    
    bool                LoadShader();
    void                CreateUniformVariables();
};

#endif // FSCOLEVEL_H