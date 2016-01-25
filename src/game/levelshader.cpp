

#include "../fracstar.h"
#include "levelshader.h"

// not great but force compiler to use our math functions
#define sqrt bigball::sqrt    
#define abs bigball::abs
#define cos bigball::cos
#define sin bigball::sin

#define UNIFORM_LEVEL( type, lvl_name, value )     UniformVariableCpp<type> lvl_name = { value, #lvl_name };

class mbox : public LevelCppAccess
{
public:
	mbox() : LevelCppAccess("mbox") 
	{
	}

#include "../../data/level/mbox/df.fs.glsl"
};

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


#undef sqrt
#undef abs
#undef cos
#undef sin