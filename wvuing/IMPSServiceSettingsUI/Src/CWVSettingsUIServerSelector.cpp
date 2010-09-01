/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Class for selecting servers
*
*/


// INCLUDE FILES
#include	"CWVSettingsUIServerSelector.h"
#include    "CWVSettingsUIDialogImp.h"
#include	<aknpopupsettingpage.h>	// CAknPopupSettingPage
#include	<badesca.h>
#include	<CWVSettingsUING.rsg>			// Application resources
#include	<cimpssapsettingsstore.h>
#include	<cimpssapsettingslist.h>
#include	<cimpssapsettings.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWVSettingsUIServerSelector::ExecuteSelectionQueryLD
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServerSelector::ExecuteSelectionQueryLD(
    CIMPSSAPSettingsStore& aSAPSettingsStore,
    CWVSettingsUIDialogImp& aDialog,
    TBool aShowDefaultSAP /*= ETrue*/,
    TBool aForceSelection /*= EFalse*/ )
    {
    CWVSettingsUIServerSelector* self = new ( ELeave ) CWVSettingsUIServerSelector(
        aSAPSettingsStore,
        aShowDefaultSAP,
        aForceSelection,
        aDialog );
    CleanupStack::PushL( self );
    self->DoSelectionL();
    CleanupStack::PopAndDestroy( self );
    }

// ---------------------------------------------------------
// CWVSettingsUIServerSelector::DoSelectionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServerSelector::DoSelectionL()
    {
    iSAPSettingsList = CIMPSSAPSettingsList::NewL();

    iSAPSettingsStore.PopulateSAPSettingsListL( *iSAPSettingsList, EIMPSIMAccessGroup );

    TInt popupResource( R_WVSETTINGSVIEW_SERVERLIST_SETTING_PAGE );
    if ( iForceSelection )
        {
        popupResource = R_WVSETTINGSVIEW_SERVERLIST_SETTING_PAGE_FORCED;
        }

    // Read server data to inner array
    TInt currentServerIndex( 0 );
    iServerList = GenerateServerListL( currentServerIndex );

    if ( !iServerList )
        {
        return;
        }

    // Initialize query array
    CAknQueryValueTextArray* textArray = CAknQueryValueTextArray::NewLC();
    textArray->SetArray( *iServerList );

    // Sets the same array and sets initial index.
    CAknQueryValueText* queryValue = CAknQueryValueText::NewLC();
    queryValue->SetArrayL( textArray );
    queryValue->SetCurrentValueIndex( currentServerIndex );

    // Creates Popup Setting Page object.
    CAknPopupSettingPage* popup =
        new( ELeave ) CAknPopupSettingPage( popupResource, *queryValue );


    // Displays Popup Setting Page screen.
    TBool res = ETrue;
    res = popup->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );

    if ( res || iForceSelection )
        {
        iSAPSettingsStore.SetToDefaultL(
            TranslateSelectedIndexL( queryValue->CurrentValueIndex() ), EIMPSIMAccessGroup );
        }

    CleanupStack::PopAndDestroy( queryValue );
    CleanupStack::PopAndDestroy( textArray );
    }

// ---------------------------------------------------------
// CWVSettingsUIServerSelector::GenerateServerListL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CDesCArrayFlat* CWVSettingsUIServerSelector::GenerateServerListL( TInt& aCurrentServerIndex )
    {

    TInt srvCount( iSAPSettingsStore.SAPCountL( EIMPSIMAccessGroup ) );

    // If there is no servers at all or there is only one server and default
    // is not shown, then the list is empty
    if ( ( srvCount == 0 ) || ( srvCount == 1 && !iShowDefaultSAP ) )
        {
        return NULL;
        }

    TUint32 defaultUID( KIMPSSettingsNullUid );
    iSAPSettingsStore.GetDefaultL( defaultUID, EIMPSIMAccessGroup );

    // If we have only 2 servers, one of them is already default and
    // default is notshown, AND we have forced selection, then we can
    // automatically choose new default server.
    if ( srvCount == 2 && !iShowDefaultSAP && iForceSelection )
        {
        if ( iSAPSettingsList->At( 0 )->Uid() == defaultUID )
            {
            iSAPSettingsStore.SetToDefaultL( iSAPSettingsList->At( 1 )->Uid(), EIMPSIMAccessGroup );
            }
        else
            {
            iSAPSettingsStore.SetToDefaultL( iSAPSettingsList->At( 0 )->Uid(), EIMPSIMAccessGroup );
            }
        return NULL;
        }

    TBool isCurrentSAPDefault( EFalse );

    //get existing server names
    CDesCArrayFlat* serverNames = new( ELeave ) CDesCArrayFlat( srvCount );
    CleanupStack::PushL( serverNames );

    for ( TInt ii( 0 ); ii < srvCount; ii++ )
        {
        isCurrentSAPDefault = iSAPSettingsList->At( ii )->Uid() == defaultUID;

        if ( !iShowDefaultSAP )
            {
            if ( !isCurrentSAPDefault )
                {
                serverNames->AppendL( iSAPSettingsList->MdcaPoint( ii ) );
                }
            }
        else
            {
            serverNames->AppendL( iSAPSettingsList->MdcaPoint( ii ) );

            if ( isCurrentSAPDefault )
                {
                aCurrentServerIndex = ii;
                }
            }

        }

    CleanupStack::Pop( serverNames );
    return serverNames;
    }

// ---------------------------------------------------------
// CWVSettingsUIServerSelector::TranslateSelectedIndexL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUint32 CWVSettingsUIServerSelector::TranslateSelectedIndexL( TInt aIndex )
    {
    TInt listIndex( 0 );
    if ( iSAPSettingsList->FindNameL( iServerList->MdcaPoint( aIndex ), listIndex ) == 0 )
        {
        return iSAPSettingsList->UidForIndex( listIndex );
        }

    return KIMPSSettingsNullUid;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUIServerSelector::CWVSettingsUIServerSelector( CIMPSSAPSettingsStore& aSAPSettingsStore,
                                                          TBool aShowDefaultSAP,
                                                          TBool aForceSelection,
                                                          CWVSettingsUIDialogImp& aDialog )
        : iSAPSettingsStore( aSAPSettingsStore ),
        iForceSelection( aForceSelection ),
        iShowDefaultSAP( aShowDefaultSAP ),
        iDialog( aDialog )
    {
    }

// Destructor
CWVSettingsUIServerSelector::~CWVSettingsUIServerSelector()
    {
    delete iServerList;
    delete iSAPSettingsList;
    }

//  End of File
