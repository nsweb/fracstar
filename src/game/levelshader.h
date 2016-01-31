


#ifndef FSLEVELSHADER_H
#define FSLEVELSHADER_H

#include "engine/component.h"
#include "gfx/shader.h"

namespace bigball
{
	class BIGBALL_API Shader;
	struct BIGBALL_API TickContext;
	struct BIGBALL_API RenderContext;
};

struct UniformVariableCppBase
{
	UniformVariableCppBase(const char* sz_name) : m_name(sz_name) {}
	virtual void	SetValue(class CoLevel* level, int var_idx) = 0;

	Name m_name;
};

////////////////////////////////////////////////////////////////////////////
class LevelCppAccess
{
public:
	LevelCppAccess(Name level_name) : m_level_name(level_name) {}
    virtual ~LevelCppAccess() {}

	virtual vec2 map(vec3 pos) = 0;
	virtual vec3 rayMarch(vec3 from, vec3 dir, float max_distance) = 0;

	void RegisterVariable(UniformVariableCppBase* v);
	void SetCppUniformValue(class CoLevel* level, Name const& var_name, int var_idx);

	Array<UniformVariableCppBase*> m_cpp_vars;
	Name m_level_name;
};

////////////////////////////////////////////////////////////////////////////
template <typename type>
struct UniformVariableCpp : public UniformVariableCppBase
{
public:
	UniformVariableCpp(type val, const char* sz_name, LevelCppAccess* Owner) : UniformVariableCppBase(sz_name), m_var(val)
	{
		Owner->RegisterVariable(this);
	}
	operator type & ()
	{
		return m_var;
	}

	virtual void SetValue(class CoLevel* level, int var_idx) override
	{
		type* typed_var = level->GetUniformBufferValue<type>(var_idx);
		m_var = *typed_var;
	}

	type m_var;

};

////////////////////////////////////////////////////////////////////////////
class LevelShader
{
public:
						LevelShader();
	virtual				~LevelShader();
	virtual void		Tick( TickContext& tick_ctxt );

	void				SetCppUniformValue(class CoLevel* level, Name const& var_name, int var_idx);

	static LevelShader*	GetStaticInstance()		{ return m_static_instance; }
	float				EstimateLevelDistance(Name const& level_name, vec3 pos);

public:
	static LevelShader*		m_static_instance;
	Array<LevelCppAccess*>	m_all_levels;
};



#endif // FSLEVELSHADER_H