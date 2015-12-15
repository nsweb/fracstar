// fseditor.cpp
//

#include "../fracstar.h"
#include "fseditor.h"

//#include "engine/entity.h"
//#include "engine/entitymanager.h"
//#include "engine/controller.h"
//#include "engine/coposition.h"
#include "ui/uimanager.h"

#include "../engine/copath.h"
#include "../engine/pathmanager.h"
#include "../game/coship.h"
#include "../game/shipmanager.h"
//#include "dfmanager.h"
//#include "fscamera.h"
////#include "../cmd/cmdbuilddata.h"

FSEditor* FSEditor::ms_peditor = nullptr;

FSEditor::FSEditor() :
    m_current_cp_edit(0)
{
    ms_peditor = this;
}

FSEditor::~FSEditor()
{
    ms_peditor = nullptr;
}

static void UIOnToggleEditorCB( bool bshow_editor )
{
    if( bshow_editor )
    {
        // Allow menu interaction
        //SDL_SetRelativeMouseMode(SDL_FALSE);
        //SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_FALSE );
        Array<CoShip*> const& ship_array = ShipManager::GetStaticInstance()->GetShipArray();
        for( int SIdx=0; SIdx < ship_array.size(); SIdx++ )
        {
            ship_array[SIdx]->ChangeState(eShipState::Edit);
        }
        
    }
    else
    {
        // Allow FPS style mouse movement
        //SDL_SetRelativeMouseMode(SDL_TRUE);
        //SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_TRUE );
        
        Array<CoShip*> const& ShipArray = ShipManager::GetStaticInstance()->GetShipArray();
        for( int SIdx=0; SIdx < ShipArray.size(); SIdx++ )
        {
            ShipArray[SIdx]->ChangeState(eShipState::Run);
        }
    }
}

static void UIDrawEditorCB( bool* bshow_editor )
{
    FSEditor::Get()->UIDrawEditor( bshow_editor );
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

void FSEditor::UIDrawEditor( bool* bshow_editor )
{
	CoShip* pShip = ShipManager::GetStaticInstance()->_GetShip();
    CoPath* pPath = PathManager::GetStaticInstance()->_GetCurrentPath();
    
    ImGui::Begin("Editor", bshow_editor, ImVec2(200,400), -1.f, 0/*ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar*/ );

#if 0
	ImGui::ShowTestWindow();
#else

	//float dist_level = pShip->m_path_dist_level;
	//float max_dist_level = pPath->m_clamped_sum_distance;

	if( ImGui::CollapsingHeader("Camera") )
	{
		ImGui::SliderFloat("PathDist", &pShip->m_path_dist_level, 0.f, pPath->m_clamped_sum_distance);
	}
	if( ImGui::CollapsingHeader("Path") )
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
		ImGui::BeginChild("Sub2", ImVec2(0,300), true);
		ImGui::Text("Control points");

		Array<String> str_cp_array;
		for( int cp_idx = 0; cp_idx < pPath->m_ctrl_points.size(); cp_idx++ )
			str_cp_array.push_back( String::Printf( "%d", cp_idx ) );

        ImGui::PushItemWidth( 50 );
		if( ImGui::ListBox( "", &m_current_cp_edit, GetItemStringArray, &str_cp_array, str_cp_array.size(), 8 ) )
		{
			if( m_current_cp_edit >= 0 && m_current_cp_edit < pPath->m_ctrl_points.size() )
			{
				float knot_dist = pPath->GetClampedKnotDistance( m_current_cp_edit );
				if( pPath->m_clamped_knot_distance > 0.f )
					pShip->m_path_dist_level = pPath->m_clamped_sum_distance * (knot_dist / pPath->m_clamped_knot_distance);
				else
					pShip->m_path_dist_level = 0.f;
			}
		}
        ImGui::PopItemWidth();

		ImGui::SameLine();
        ImGui::BeginChild("Action", ImVec2(0,100), true);
        if( m_current_cp_edit >= 0 && m_current_cp_edit < pPath->m_ctrl_points.size() )
        {
            if( ImGui::Button( "here" ) )
            {
                float knot_dist = pPath->GetClampedKnotDistance( m_current_cp_edit );
                if( pPath->m_clamped_knot_distance > 0.f )
                    pShip->m_path_dist_level = pPath->m_clamped_sum_distance * (knot_dist / pPath->m_clamped_knot_distance);
                else
                    pShip->m_path_dist_level = 0.f;
            }
            ImGui::SameLine();
            bool insert_before = ImGui::Button( "ins. before" );
            ImGui::SameLine();
            bool insert_after = ImGui::Button( "ins. after" );
            if( insert_before || insert_after )
            {
                pPath->InsertControlPoint( m_current_cp_edit, insert_after );
            }
            ImGui::SameLine();
            bool edit_button = ImGui::Button( "edit" );
            if( edit_button )
            {
                if( !m_edit_mode )
                {
                    // Open edit mode
                }
                else
                {
                    // Close edit mode
                }
                m_edit_mode = !m_edit_mode;
            }
            ImGui::SameLine();
            // disable if one remaining ctrl point...
            if( pPath->m_ctrl_points.size() > 1 && ImGui::Button( "del" ) )
            {
                pPath->DeleteControlPoint( m_current_cp_edit );
            }
            if( m_edit_mode)
            {
                ImGui::SliderFloat("slide", &m_edit_slide, -1.f, 1.f);
                ImGui::InputFloat3("pos", (float*)&pPath->m_ctrl_points[m_current_cp_edit].m_pos);
                ImGui::InputFloat("knot", (float*)&pPath->m_ctrl_points[m_current_cp_edit].m_knot);
                //static float val = 0.f;
                //ImGui::VSliderFloat("slide2", ImVec2(20,50),  &val, 0.f, 1.f);
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
