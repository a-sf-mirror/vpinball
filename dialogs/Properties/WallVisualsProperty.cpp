#include "stdafx.h"
#include "Properties/WallVisualsProperty.h"
#include <WindowsX.h>

WallVisualsProperty::WallVisualsProperty(VectorProtected<ISelect> *pvsel) : BasePropertyDialog(IDD_PROPWALL_VISUALS, pvsel)
{
}

void WallVisualsProperty::UpdateVisuals()
{
    for (int i = 0; i < m_pvsel->Size(); i++)
    {
        Surface * const wall = (Surface*)m_pvsel->ElementAt(i);
        PropertyDialog::UpdateTextureComboBox(wall->GetPTable()->GetImageList(), m_sideImageCombo, wall->m_d.m_szSideImage);
        PropertyDialog::UpdateMaterialComboBox(wall->GetPTable()->GetMaterialList(), m_topMaterialCombo, wall->m_d.m_szTopMaterial);
        PropertyDialog::UpdateMaterialComboBox(wall->GetPTable()->GetMaterialList(), m_sideMaterialCombo, wall->m_d.m_szSideMaterial);
        PropertyDialog::UpdateMaterialComboBox(wall->GetPTable()->GetMaterialList(), m_slingshotMaterialCombo, wall->m_d.m_szSlingShotMaterial);
        PropertyDialog::SetCheckboxState(::GetDlgItem(GetHwnd(), 16), wall->m_d.m_topBottomVisible);
        PropertyDialog::SetCheckboxState(::GetDlgItem(GetHwnd(), 13), wall->m_d.m_displayTexture);
        PropertyDialog::SetCheckboxState(::GetDlgItem(GetHwnd(), 109), wall->m_d.m_sideVisible);
        PropertyDialog::SetCheckboxState(::GetDlgItem(GetHwnd(), 112), wall->m_d.m_slingshotAnimation);
        PropertyDialog::SetCheckboxState(::GetDlgItem(GetHwnd(), 113), wall->m_d.m_flipbook);
        PropertyDialog::SetFloatTextbox(m_disableLightingEdit, wall->m_d.m_disableLightingTop);
        PropertyDialog::SetFloatTextbox(m_disableLightFromBelowEdit, wall->m_d.m_disableLightingBelow);
        PropertyDialog::SetFloatTextbox(m_topHeightEdit, wall->m_d.m_heighttop);
        PropertyDialog::SetFloatTextbox(m_bottomHeightEdit, wall->m_d.m_heightbottom);
        UpdateBaseVisuals(wall, &wall->m_d);
        //only show the first element on multi-select
        break;
    }
}

void WallVisualsProperty::UpdateProperties(const int dispid)
{
    for (int i = 0; i < m_pvsel->Size(); i++)
    {
        Surface * const wall = (Surface*)m_pvsel->ElementAt(i);
        switch (dispid)
        {
            case 9:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_heighttop = PropertyDialog::GetFloatTextbox(m_topHeightEdit);
                PropertyDialog::EndUndo(wall);
                break;
            case 8:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_heightbottom = PropertyDialog::GetFloatTextbox(m_bottomHeightEdit);
                PropertyDialog::EndUndo(wall);
                break;
            case DISPID_Image2:
                PropertyDialog::StartUndo(wall);
                PropertyDialog::GetComboBoxText(m_sideImageCombo, wall->m_d.m_szSideImage);
                PropertyDialog::EndUndo(wall);
                break;
            case IDC_MATERIAL_COMBO:
                PropertyDialog::StartUndo(wall);
                PropertyDialog::GetComboBoxText(m_topMaterialCombo, wall->m_d.m_szTopMaterial);
                PropertyDialog::EndUndo(wall);
                break;
            case IDC_MATERIAL_COMBO2:
                PropertyDialog::StartUndo(wall);
                PropertyDialog::GetComboBoxText(m_sideMaterialCombo, wall->m_d.m_szSideMaterial);
                PropertyDialog::EndUndo(wall);
                break;
            case IDC_MATERIAL_COMBO3:
                PropertyDialog::StartUndo(wall);
                PropertyDialog::GetComboBoxText(m_slingshotMaterialCombo, wall->m_d.m_szSlingShotMaterial);
                PropertyDialog::EndUndo(wall);
                break;
            case IDC_BLEND_DISABLE_LIGHTING:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_disableLightingTop = PropertyDialog::GetFloatTextbox(m_disableLightingEdit);
                PropertyDialog::EndUndo(wall);
                break;
            case IDC_BLEND_DISABLE_LIGHTING_FROM_BELOW:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_disableLightingBelow = PropertyDialog::GetFloatTextbox(m_disableLightFromBelowEdit);
                PropertyDialog::EndUndo(wall);
                break;
            case 16:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_topBottomVisible = PropertyDialog::GetCheckboxState(::GetDlgItem(GetHwnd(), dispid));
                PropertyDialog::EndUndo(wall);
                break;
            case 13:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_displayTexture = PropertyDialog::GetCheckboxState(::GetDlgItem(GetHwnd(), dispid));
                PropertyDialog::EndUndo(wall);
                break;
            case 109:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_sideVisible = PropertyDialog::GetCheckboxState(::GetDlgItem(GetHwnd(), dispid));
                PropertyDialog::EndUndo(wall);
                break;
            case 112:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_slingshotAnimation = PropertyDialog::GetCheckboxState(::GetDlgItem(GetHwnd(), dispid));
                PropertyDialog::EndUndo(wall);
                break;
            case 113:
                PropertyDialog::StartUndo(wall);
                wall->m_d.m_flipbook = PropertyDialog::GetCheckboxState(::GetDlgItem(GetHwnd(), dispid));
                PropertyDialog::EndUndo(wall);
                break;
            default:
                UpdateBaseProperties(wall, &wall->m_d, dispid);
                break;
        }
    }
    UpdateVisuals();
}

BOOL WallVisualsProperty::OnInitDialog()
{
    AttachItem(DISPID_Image, m_topImageCombo);
    AttachItem(DISPID_Image2, m_sideImageCombo);
    AttachItem(IDC_MATERIAL_COMBO, m_topMaterialCombo);
    AttachItem(IDC_MATERIAL_COMBO2, m_sideMaterialCombo);
    AttachItem(IDC_MATERIAL_COMBO3, m_slingshotMaterialCombo);
    AttachItem(IDC_BLEND_DISABLE_LIGHTING, m_disableLightingEdit);
    AttachItem(IDC_BLEND_DISABLE_LIGHTING_FROM_BELOW, m_disableLightFromBelowEdit);
    AttachItem(9, m_topHeightEdit);
    AttachItem(8, m_bottomHeightEdit);

    m_baseImageCombo = &m_topMaterialCombo;
    m_baseMaterialCombo = &m_topMaterialCombo;
    m_hReflectionEnabledCheck = ::GetDlgItem(GetHwnd(), IDC_REFLECT_ENABLED_CHECK);

    UpdateVisuals();
    return TRUE;
}

