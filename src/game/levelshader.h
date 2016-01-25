


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

////////////////////////////////////////////////////////////////////////////
template <typename type>
struct UniformVariableCpp
{
public:
	UniformVariableCpp(type val, const char* sz_name) : m_var(val), m_name(sz_name)
	{
		LevelShader::GetStaticInstance()->RegisterVariable(this);
	}
	operator type & ()
	{
		return m_var;
	}

	type m_var;
	Name m_name;
};

////////////////////////////////////////////////////////////////////////////
class LevelCppAccess
{
public:
	LevelCppAccess(Name level_name) : m_level_name(level_name) {}

	virtual vec2 map(vec3 Pos) = 0;
	virtual vec3 rayMarch(vec3 from, vec3 dir, float MaxDistance) = 0;

	Name m_level_name;
};

////////////////////////////////////////////////////////////////////////////
class LevelShader
{
public:
						LevelShader();
	virtual				~LevelShader();
	virtual void		Tick( TickContext& tick_ctxt );

	template <typename type>
	static void RegisterVariable(UniformVariableCpp<type>* v)
	{

	}

	static LevelShader*	GetStaticInstance()		{ return m_static_instance; }

public:
	static LevelShader*		m_static_instance;
	Array<LevelCppAccess*>	m_all_levels;
};

#endif // FSLEVELSHADER_H