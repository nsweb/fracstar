#ifndef FSCOLEVEL_H
#define FSCOLEVEL_H

//#include "bgfx_utils.h"
#include "engine/component.h"
//#include "gfx/shader.h"

namespace bigfx
{
	class BIGFX_API Shader;
	struct BIGFX_API TickContext;
	struct BIGFX_API RenderContext;
};

class CoLevel;
template <typename type> class LevelVariableTrack;

// (REBIND)
//class UniformVariable : public ShaderUniformDetail
//{
//public:
//    int     m_buffer_offset;
//};

class LevelVariableTrackBase
{
public:
    int		m_var_index;
	Name	m_var_name;
    
    virtual int GetVarSize() const = 0;
    virtual void RetrieveShaderUniformValue(bgfx::ShaderHandle shader, bgfx::UniformHandle const& uni, void* var_buffer ) = 0;
	virtual void InterpUniformValue(float time, class CoLevel* parent_level) = 0;
	virtual void SetShaderUniformValue(bgfx::ShaderHandle, class CoLevel* parent_level) = 0;
};

//////////////////////////////////////////////////
class CoLevel : public Component 
{
	CLASS_EQUIP_H(CoLevel, Component)

public:
						CoLevel();
	virtual				~CoLevel();

	static Component*	NewComponent()		{ return new CoLevel();	}

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr ) override;
	virtual void		Destroy() override;
	virtual void		AddToWorld() override;
	virtual void		RemoveFromWorld() override;
	virtual void		Tick( TickContext& tick_ctxt );
	void				InterpAndSetUniforms( float time );
	//void				_Render( RenderContext& RenderCtxt, Shader* BlockShader );

	template <typename type>
	type* GetUniformBufferValue(int var_idx)
	{
		int buf_location = m_shader_variables[var_idx].m_buffer_offset;
		return (type*)&m_uniform_buffer[buf_location];
	}

public:
    Name                            m_level_name;
    bgfx::ProgramHandle             m_df_program;
    bgfx::ShaderHandle              m_df_vsh;
    bgfx::ShaderHandle              m_df_fsh;
    bgfx::ProgramHandle             m_hit_program;

    // (REBIND)
    //Array<UniformVariable>          m_shader_variables;
    Array<uint8>                    m_uniform_buffer;
    Array<LevelVariableTrackBase*>  m_var_tracks;
    
    bool                LoadShaders();
    void                CreateUniformVariables();
};

//////////////////////////////////////////////////////
template <typename type>
class LevelVariableTrack : public LevelVariableTrackBase
{
public:
    struct Key
    {
        float       m_time;
        type     m_value;
    };
    
    Array<Key>  m_keys;
    
    virtual int GetVarSize() const override  { return sizeof(type); }
    virtual void RetrieveShaderUniformValue(bgfx::ProgramHandle shader, bgfx::UniformHandle const& uni, void* var_buffer) override
    {
        shader->GetUniform(uni, *(type*)var_buffer);
    }
    
    //VarType* GetUniformBufferValue( Array<UniformVariable> const& vars, Array<uint8> const& uni_buffer )
    //{
    //	int buf_location = vars[m_var_index].m_buffer_offset;
    //	return (VarType*)&uni_buffer[buf_location];
    //}
    
    virtual void InterpUniformValue( float time, class CoLevel* parent_level ) override
    {
        if( !m_keys.size() )
        return;
        
        time = bigfx::clamp( time, 0.f, m_keys.Last().m_time );
        
        type* value_addr = parent_level->GetUniformBufferValue<type>(m_var_index);
        if( m_keys.size() == 1 )
        {
            *value_addr = m_keys[0].m_value;
            return;
        }
        
        int found_key_idx = 0;
        for( int key_idx = 1; key_idx < m_keys.size(); key_idx++ )
        {
            if( time <= m_keys[key_idx].m_time )
            {
                found_key_idx = key_idx - 1;
                break;
            }
        }
        
        float time_ratio = (time - m_keys[found_key_idx].m_time) / (m_keys[found_key_idx + 1].m_time - m_keys[found_key_idx].m_time);
        *value_addr = bigfx::lerp( m_keys[found_key_idx].m_value, m_keys[found_key_idx + 1].m_value, time_ratio );
    }
    
    virtual void SetShaderUniformValue(bgfx::ShaderHandle shader, class CoLevel* parent_level) override
    {
        if( !m_keys.size() )
        return;
        
        type* value_addr = parent_level->GetUniformBufferValue<type>(m_var_index);
        shader->SetUniform(parent_level->m_shader_variables[m_var_index], *value_addr);
    }
};

#endif // FSCOLEVEL_H