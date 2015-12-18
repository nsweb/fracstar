

#ifndef FSCAMERA_H
#define FSCAMERA_H

#include "engine/camera.h"
#include "../engine/copath.h"

//////////////////////////////////////////////////////////////////////////

class BIGBALL_API FSCameraCtrl_Fly : public CameraCtrl_Base
{
	CLASS_EQUIP_H(FSCameraCtrl_Fly, CameraCtrl_Base)

public:
					FSCameraCtrl_Fly();

	//float			m_StrafeSpeed;
	//float			m_RotationSpeed;

public:
	virtual void		UpdateView( CameraView& cam_view, float delta_seconds );
	virtual bool		OnControllerInput( Camera* pcamera, struct ControllerInput const& input );
	void				SetTarget( Entity* ptarget )		{ m_ptarget = ptarget; }

protected:
	Entity*			m_ptarget;
};

class BIGBALL_API FSCameraCtrl_EditPath : public CameraCtrl_Base
{
	CLASS_EQUIP_H(FSCameraCtrl_EditPath, CameraCtrl_Base)

public:
	FSCameraCtrl_EditPath();

	virtual void		UpdateView( CameraView& cam_view, float delta_seconds );
	virtual bool		OnControllerInput( Camera* pcamera, struct ControllerInput const& input );
	void				SetTarget( CoPath* ptarget )		{ m_ptarget = ptarget; }
	void				ResetEdit( int at_cp_idx );
	void				ResetEdit( float nearest_dist_level );
	void				BuildGui();

public:
	float			m_strafe_speed;
	float			m_rotation_speed;
	CoPath*			m_ptarget;

    ControlPoint    m_backup_cp;
	int             m_current_cp_edit;
	float           m_edit_slide;
	vec3			m_interp_pos;
	vec3			m_interp_tan;
	float			m_dist_before;
	float			m_dist_after;
};

#endif // FSCAMERA_H