
#include "../fracstar.h"
#include "fscamera.h"
#include "engine/coposition.h"
#include "engine/controller.h"



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

bool FSCameraCtrl_Fly::OnControllerInput( Camera* pcamera, ControllerInput const& input )
{
	return Super::OnControllerInput( pcamera, input );
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
    float knot_dist = m_ptarget->GetSumKnotDistance( m_current_cp_edit );
    knot_dist += m_edit_slide;
    
    transform tf;
    m_ptarget->InterpPathKnotDist( knot_dist, tf );
	m_interp_pos = tf.GetTranslation();
	m_interp_tan = tf.TransformVector( vec3(0.f,0.f,-1.f) );
	

    cam_view.m_Transform.SetTranslation( tf.GetTranslation() );
    cam_view.m_Transform.SetRotation( tf.GetRotation() );

	Super::UpdateView( cam_view, delta_seconds );
}

bool FSCameraCtrl_EditPath::OnControllerInput( Camera* pcamera, ControllerInput const& input )
{
    CameraView& cam_view = pcamera->GetView();
    
    if( input.m_type == eCIT_Key )
    {
        mat4 cam_to_world( cam_view.m_Transform.GetRotation() );
        vec3 right = cam_to_world.v0.xyz;
        vec3 up = cam_to_world.v1.xyz;
        vec3 front = -cam_to_world.v2.xyz;
        
        vec3& cp_pos = m_ptarget->m_ctrl_points[m_current_cp_edit].m_pos;
        
        cp_pos += (right * input.m_delta.x + up * input.m_delta.z + front * input.m_delta.y) * m_strafe_speed;
        m_ptarget->OnControlPointChanged( m_current_cp_edit );
    }
    else if( input.m_type == eCIT_Mouse )
    {
        // TODO : arcball rotation
        //quat YawPitchRoll( quat::fromeuler_xyz( Input.m_Delta.y * m_RotationSpeed, Input.m_Delta.x * m_RotationSpeed, 0.0f ) );
        //View.m_Transform.SetRotation( View.m_Transform.GetRotation() * YawPitchRoll );
    }
    
	return Super::OnControllerInput( pcamera, input );
}

void FSCameraCtrl_EditPath::ResetEdit( int at_cp_idx )
{
	m_current_cp_edit = at_cp_idx;
	m_edit_slide = 0.f;
    
    // Back up control point params, in case the modifications are cancelled
    m_backup_cp = m_ptarget->m_ctrl_points[at_cp_idx];
}
void FSCameraCtrl_EditPath::ResetEdit( float nearest_dist_level )
{

}

void FSCameraCtrl_EditPath::BuildGui()
{
	ImGui::SliderFloat("slide", &m_edit_slide, -1.f, 1.f);
	ImGui::SameLine();
	if( ImGui::Button("reset slider") )
		m_edit_slide = 0.f;
	if( ImGui::InputFloat3("cp_pos", (float*)&m_ptarget->m_ctrl_points[m_current_cp_edit].m_pos) )
    {
        m_ptarget->OnControlPointChanged( m_current_cp_edit );
    }
	ImGui::InputFloat("knot", (float*)&m_ptarget->m_ctrl_points[m_current_cp_edit].m_knot);

	ImGui::InputFloat3("cam_pos", (float*)&m_interp_pos, -1, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("cam_dir", (float*)&m_interp_tan, -1, ImGuiInputTextFlags_ReadOnly);

	m_dist_before = m_ptarget->GetSumDistance(m_current_cp_edit) - m_ptarget->GetSumDistance(m_current_cp_edit-1);
	m_dist_after = m_ptarget->GetSumDistance(m_current_cp_edit+1) - m_ptarget->GetSumDistance(m_current_cp_edit);
	ImGui::InputFloat("dist_before", (float*)&m_dist_before, -1, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("dist_after", (float*)&m_dist_after, -1, ImGuiInputTextFlags_ReadOnly);

	//static float val = 0.f;
	//ImGui::VSliderFloat("slide2", ImVec2(20,50),  &val, 0.f, 1.f);
}
