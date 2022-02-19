

#include "../fracstar.h"
#include "levelshader.h"
#include "colevel.h"

// not great but force compiler to use our math functions
#define sqrt bx::sqrt
#define abs bx::abs
#define cos bx::cos
#define sin bx::sin

#define UNIFORM_LEVEL( type, lvl_name, value )     UniformVariableCpp<type> lvl_name = { (type)value, #lvl_name, this };

class mbox : public LevelCppAccess
{
public:
	mbox() : LevelCppAccess("mbox") 
	{
	}

#include "../../data/level/mbox/df_fs.sc"
};

////////////////////////////////////////////////////////////////

void LevelCppAccess::RegisterVariable(UniformVariableCppBase* v)
{
	m_cpp_vars.push_back(v);
}
void LevelCppAccess::SetCppUniformValue(class CoLevel* level, Name const& var_name, int var_idx)
{
	for (int idx = 0; idx < m_cpp_vars.size(); idx++)
	{
		if (m_cpp_vars[idx]->m_name == var_name)
		{
			m_cpp_vars[idx]->SetValue(level, var_idx);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////

LevelShader* LevelShader::m_static_instance = nullptr;

LevelShader::LevelShader()
{
	m_static_instance = this;

	m_all_levels.push_back(new mbox());
}

LevelShader::~LevelShader()
{
	m_static_instance = nullptr;

	for (int level_idx = 0; level_idx < m_all_levels.size(); level_idx++)
	{
		BB_DELETE(m_all_levels[level_idx]);
	}
}

void LevelShader::Tick(TickContext& tick_ctxt)
{

}

void LevelShader::SetCppUniformValue(CoLevel* level, Name const& var_name, int var_idx)
{
	for (int level_idx = 0; level_idx < m_all_levels.size(); level_idx++)
	{
		if (m_all_levels[level_idx]->m_level_name == level->m_level_name)
		{
			m_all_levels[level_idx]->SetCppUniformValue(level, var_name, var_idx);
			break;
		}
	}
}

float LevelShader::EstimateLevelDistance(Name const& level_name, vec3 pos)
{
	for (int level_idx = 0; level_idx < m_all_levels.size(); level_idx++)
	{
		if (m_all_levels[level_idx]->m_level_name == level_name)
		{
			vec2 map_val = m_all_levels[level_idx]->map(pos);
			return map_val.x;
		}
	}

	return 1e8f;
}

vec3 LevelShader::RayMarch(Name const& level_name, vec3 ray_pos, vec3 ray_dir, float max_dist)
{
    vec3 pos_end = ray_pos;
    for (int level_idx = 0; level_idx < m_all_levels.size(); level_idx++)
    {
        if (m_all_levels[level_idx]->m_level_name == level_name)
        {
            vec3 march_res = m_all_levels[level_idx]->rayMarch(ray_pos, ray_dir, max_dist);
            pos_end = ray_pos + ray_dir * march_res.x;
        }
    }
    
    return pos_end;
}


#undef sqrt
#undef abs
#undef cos
#undef sin