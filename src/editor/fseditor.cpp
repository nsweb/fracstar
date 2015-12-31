// fseditor.cpp
//

#include "../fracstar.h"
#include "fseditor.h"

//#include "engine/entity.h"
//#include "engine/entitymanager.h"
#include "engine/controller.h"
//#include "engine/coposition.h"
#include "ui/uimanager.h"
#include "gfx/rendercontext.h"

#include "../engine/copath.h"
#include "../engine/pathmanager.h"
#include "../engine/fscamera.h"
#include "../game/coship.h"
#include "../game/shipmanager.h"
#include "../game/fsworld.h"
#include "../game/colevel.h"
//#include "dfmanager.h"

FSEditor* FSEditor::ms_peditor = nullptr;

FSEditor::FSEditor() :
    m_current_cp_idx(0)
{
    ms_peditor = this;
}

FSEditor::~FSEditor()
{
    ms_peditor = nullptr;
}

static void UIOnToggleEditorCB( bool bshow_editor )
{
    CoShip* pcoship = ShipManager::GetStaticInstance()->_GetShip();
    CoLevel* level = FSWorld::GetStaticInstance()->GetCurrentLevel();
    CoPath* pcopath = level ? static_cast<CoPath*>( level->GetEntityComponent( CoPath::StaticClass() ) ) : nullptr;
    
    if( bshow_editor )
    {
        // Allow menu interaction
        //SDL_SetRelativeMouseMode(SDL_FALSE);
        //SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_FALSE );
        pcoship->ChangeState(eShipState::Edit);
        
        // Open edit mode camera
        Controller::GetStaticInstance()->SetActiveCameraCtrl( "FSCameraCtrl_EditPath" );
        FSCameraCtrl_EditPath* cam_edit = static_cast<FSCameraCtrl_EditPath*>( Controller::GetStaticInstance()->GetActiveCameraCtrl() );
        if( cam_edit )
        {
            cam_edit->SetTarget( pcopath );
            FSEditor::Get()->m_current_cp_idx = cam_edit->ResetEdit( pcoship->m_path_knot_dist_level );
            //cam_edit->ResetEdit( m_current_cp_idx );
        }
    }
    else
    {
        // Allow FPS style mouse movement
        //SDL_SetRelativeMouseMode(SDL_TRUE);
        //SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_TRUE );
        pcoship->ChangeState(eShipState::Run);
        
        // Close edit mode camera
        Controller::GetStaticInstance()->SetActiveCameraCtrl( "FSCameraCtrl_Fly" );
    }
}

static void UIDrawEditorCB( bool* bshow_editor, bigball::RenderContext& render_ctxt )
{
    FSEditor::Get()->UIDrawEditor( bshow_editor, render_ctxt );
}

bool FSEditor::GetItemStringArray( void* data, int idx, const char** out_text )
{
	Array<String>* str_array = (Array<String>*)data;
	if( idx >= 0 && idx < str_array->size() )
	{
		*out_text = (*str_array)[idx].c_str();
		return true;
	}

	return false;
}

void FSEditor::UIDrawEditor( bool* bshow_editor, RenderContext& render_ctxt )
{
	CoShip* pcoship = ShipManager::GetStaticInstance()->_GetShip();
    CoLevel* level = FSWorld::GetStaticInstance()->GetCurrentLevel();
    CoPath* pcopath = level ? static_cast<CoPath*>( level->GetEntityComponent( CoPath::StaticClass() ) ) : nullptr;
    
    CameraCtrl_Base* cam_ctrl = Controller::GetStaticInstance()->GetActiveCameraCtrl();
    FSCameraCtrl_EditPath* cam_edit = nullptr;
    if( cam_ctrl && cam_ctrl->IsA( FSCameraCtrl_EditPath::StaticClass() ) )
        cam_edit = static_cast<FSCameraCtrl_EditPath*>( cam_ctrl );
    
    ImGui::Begin("Editor", bshow_editor, ImVec2(200,400), -1.f, 0/*ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar*/ );

#if 0
	ImGui::ShowTestWindow();
#else

    if( ImGui::CollapsingHeader("Camera") )
    {
        ImGui::SliderFloat("strafe_speed", &cam_edit->m_strafe_speed, 0.f, 2.f);
        bool left = ImGui::Button( "left" );
        ImGui::SameLine();
        bool right = ImGui::Button( "right" );
        ImGui::SameLine();
        bool front = ImGui::Button( "front" );
        ImGui::SameLine();
        bool back = ImGui::Button( "back" );
        ImGui::SameLine();
        bool up = ImGui::Button( "up" );
        ImGui::SameLine();
        bool down = ImGui::Button( "down" );
        
        uint32 modifiers = 0;
        if( left )
            Controller::GetStaticInstance()->OnInputX( modifiers, -render_ctxt.m_delta_seconds );
        if( right )
            Controller::GetStaticInstance()->OnInputX( modifiers, render_ctxt.m_delta_seconds );
        if( front )
            Controller::GetStaticInstance()->OnInputY( modifiers, render_ctxt.m_delta_seconds );
        if( back )
            Controller::GetStaticInstance()->OnInputY( modifiers, -render_ctxt.m_delta_seconds );
        if( up )
            Controller::GetStaticInstance()->OnInputZ( modifiers, render_ctxt.m_delta_seconds );
        if( down )
            Controller::GetStaticInstance()->OnInputZ( modifiers, -render_ctxt.m_delta_seconds );
    }
	if( ImGui::CollapsingHeader("Path") )
	{
        bool save_path = ImGui::Button( "save" );
        ImGui::SameLine();
        bool load_path = ImGui::Button( "load" );
        if( save_path || load_path )
        {
            bigball::File lvl_path;
            String str_file = String::Printf("../data/level/%s/path.fs", pcopath->m_level_name.ToString().c_str());
            if( lvl_path.Open( str_file.c_str(), save_path ) )
            {
                pcopath->Serialize(lvl_path);
            }
        }
        
        if( ImGui::SliderFloat("knot_dist", &pcoship->m_path_knot_dist_level, 0.f, pcopath->m_sum_knot_distance) )
        {
            pcoship->m_path_knot_dist_level = bigball::clamp( pcoship->m_path_knot_dist_level, 0.f, pcopath->m_sum_knot_distance );
            if( cam_edit )
                m_current_cp_idx = cam_edit->ResetEdit( pcoship->m_path_knot_dist_level );
        }
        ImGui::InputFloat("sum_dist", &pcoship->m_path_dist_level, -1, ImGuiInputTextFlags_ReadOnly);
        
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
		ImGui::BeginChild("Sub2", ImVec2(0,400), true);
		ImGui::Text("Control points");

		Array<String> str_cp_array;
		for( int cp_idx = 0; cp_idx < pcopath->m_ctrl_points.size(); cp_idx++ )
			str_cp_array.push_back( String::Printf( "%d", cp_idx ) );

        ImGui::PushItemWidth( 50 );
		if( ImGui::ListBox( "", &m_current_cp_idx, GetItemStringArray, &str_cp_array, str_cp_array.size(), 11 ) )
		{
			if( m_current_cp_idx >= 0 && m_current_cp_idx < pcopath->m_ctrl_points.size() )
			{
				pcoship->m_path_knot_dist_level = pcopath->GetSumKnotDistance( m_current_cp_idx );
				if( cam_edit )
					m_current_cp_idx = cam_edit->ResetEdit( pcoship->m_path_knot_dist_level );
			}
		}
        ImGui::PopItemWidth();

		ImGui::SameLine();
        ImGui::BeginChild("Action", ImVec2(0,200), true);
        if( m_current_cp_idx >= 0 && m_current_cp_idx < pcopath->m_ctrl_points.size() )
        {
            bool insert_before = ImGui::Button( "ins. before" );
            ImGui::SameLine();
            bool insert_after = ImGui::Button( "ins. after" );
            if( insert_before || insert_after )
            {
                pcopath->InsertControlPoint( m_current_cp_idx, insert_after );
            }
            
            float delta = cam_edit ? cam_edit->m_edit_slide : 0.f;
            if( pcopath->m_ctrl_points.size() > 1 && (delta > 1e-2f || delta < -1e-2f) )
            {
                ImGui::SameLine();
                if( ImGui::Button( "ins. here" ) )
                {
                    pcopath->InsertControlPoint( pcopath->GetSumKnotDistance( m_current_cp_idx ) + delta );
                }
            }

            // disable if one remaining ctrl point...
            if( pcopath->m_ctrl_points.size() > 1 )
            {
                ImGui::SameLine();
                if( ImGui::Button( "del" ) )
                {
                    pcopath->DeleteControlPoint( m_current_cp_idx );
                    if( cam_edit )
                        m_current_cp_idx = cam_edit->ResetEdit( pcoship->m_path_knot_dist_level );
                }
            }
            if( cam_edit)
            {
				cam_edit->BuildGui();
            }
        }
        ImGui::EndChild();  // Action
		ImGui::EndChild();  // Sub2
		ImGui::PopStyleVar();

	}
    
#endif
    ImGui::End();
}

bool FSEditor::Init()
{
	UIManager::GetStaticInstance()->SetDrawEditorFn( &UIDrawEditorCB );
	UIManager::GetStaticInstance()->SetToggleEditorFn( &UIOnToggleEditorCB );

	return true;
}

void FSEditor::Shutdown()
{

}
