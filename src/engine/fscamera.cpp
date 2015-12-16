
#include "../fracstar.h"
#include "fscamera.h"
#include "engine/coposition.h"
#include "../engine/copath.h"


CLASS_EQUIP_CPP(FSCameraCtrl_Fly);
CLASS_EQUIP_CPP(FSCameraCtrl_EditPath);

FSCameraCtrl_Fly::FSCameraCtrl_Fly() :
	m_ptarget(nullptr)
{

}

void FSCameraCtrl_Fly::UpdateView( CameraView& cam_view, float delta_seconds )
{
	// Retrieve frame along path (ship coposition)
	CoPosition* target_copos = static_cast<CoPosition*>( m_ptarget ? m_ptarget->GetComponent( CoPosition::StaticClass() ) : nullptr );
	cam_view.m_Transform.SetTranslation( target_copos->GetPosition() );
	cam_view.m_Transform.SetRotation( target_copos->GetRotation() );


	//CoDPosition* pTargetCoPos = static_cast<CoDPosition*>( m_ptargetPlanet ? m_ptargetPlanet->GetComponent( CoDPosition::StaticClass() ) : nullptr );
	//CoBlocks* pTargetCoBlocks = static_cast<CoBlocks*>( m_ptargetPlanet ? m_ptargetPlanet->GetComponent( CoBlocks::StaticClass() ) : nullptr );
	//if( pTargetCoPos && pTargetCoBlocks )
	//{
	//	// Update speed depending on distance
	//	dvec3 CamPos = cam_view.m_Transform.GetTranslation();
	//	float GroundDist = pTargetCoBlocks->ComputeGroundSignedDistance( CamPos );
	//	
	//	//dvec3 TargetPos = pTargetCoPos->GetPosition();
	//	//float Dist = bigball::length( CamPos - TargetPos );

	//	double OctreeSize = pTargetCoBlocks->GetOctreeSize();
	//	double MaxDist = 10.0 * OctreeSize;
	//	float DistRatio = bigball::clamp( (float)((GroundDist) / MaxDist), 0.0f, 1.0f );
	//	m_StrafeSpeed = bigball::lerp( 1.0f, 4.0f * (float)OctreeSize, DistRatio );

	//	// Adjust near and far planes
	//	float NearFarRatio = 100000.0f;
	//	static float reduce = 0.5f;
	//	float NearD = bigball::max( 1.0f, (float)((GroundDist) * reduce) );
	//	cam_view.m_fParameters[eCP_NEAR] = NearD;
	//	cam_view.m_fParameters[eCP_FAR] = NearD * NearFarRatio;

	//	if( g_pEngine->m_FrameCount % 60 == 0 )
	//		BB_LOG( Test, Log, "Distance to target = %f - NearPlane = %f", (float)(GroundDist), NearD );
	//}

	Super::UpdateView( cam_view, delta_seconds );
}

bool FSCameraCtrl_Fly::OnControllerInput( Camera* pCamera, ControllerInput const& Input )
{
	return Super::OnControllerInput( pCamera, Input );
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
FSCameraCtrl_EditPath::FSCameraCtrl_EditPath() :
		m_strafe_speed(1.f),
		m_rotation_speed(1.f),
		m_ptarget(nullptr),
		m_current_cp_edit(0),
		m_edit_slide(0.f)
{

}


void FSCameraCtrl_EditPath::UpdateView( CameraView& cam_view, float delta_seconds )
{
	// Retrieve frame along path (ship coposition)
	//CoPosition* pTargetCoPos = static_cast<CoPosition*>( m_ptarget ? m_ptarget->GetComponent( CoPosition::StaticClass() ) : nullptr );
	//cam_view.m_Transform.SetTranslation( pTargetCoPos->GetPosition() );
	//cam_view.m_Transform.SetRotation( pTargetCoPos->GetRotation() );

	Super::UpdateView( cam_view, delta_seconds );
}

bool FSCameraCtrl_EditPath::OnControllerInput( Camera* pCamera, ControllerInput const& Input )
{
	return Super::OnControllerInput( pCamera, Input );
}

void FSCameraCtrl_EditPath::ResetEdit( int at_cp_idx )
{
	m_current_cp_edit = at_cp_idx;
}
void FSCameraCtrl_EditPath::ResetEdit( float nearest_dist_level )
{

}

void FSCameraCtrl_EditPath::BuildGui()
{
	ImGui::SliderFloat("slide", &m_edit_slide, -1.f, 1.f);
	ImGui::InputFloat3("pos", (float*)&m_ptarget->m_ctrl_points[m_current_cp_edit].m_pos);
	ImGui::InputFloat("knot", (float*)&m_ptarget->m_ctrl_points[m_current_cp_edit].m_knot);
	//static float val = 0.f;
	//ImGui::VSliderFloat("slide2", ImVec2(20,50),  &val, 0.f, 1.f);
}
