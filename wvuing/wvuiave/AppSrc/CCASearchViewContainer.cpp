/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for search view
*
*/



// INCLUDE FILES
#include    "CCASearchViewContainer.h"
#include    "ChatDefinitions.h"
#include    "CCAStatusPaneHandler.h"
#include    "MCAViewSwitcher.h"
#include    "CCAAppUi.h"
#include    "ChatDebugPrint.h"
#include    "IMNoteMapper.h"
#include	"CCAUINGUtils.h"
#include    "IMDialogUtils.h"
#include    "CCAGroupUtils.h"
#include    "IMUtils.h"
#include    "MCARecipientObserver.h"
#include    "CCAMultiLineDataQueryDialog.h"
#include    "CCABlockingUI.h"
#include    "CCAInviteEditor.h"
#include    "CCAContactEditor.h"
#include    "CCAChatListView.h"
#include    "CAExternalInterface.h"
#include    "MCAServerContactsArrayPC.h"
#include    "MCASearchInterfacePC.h"
#include    "MCABlockingPC.h"
#include 	"MCASessionHandlerCmd.h"
#include    "CCACommandManagerFactory.h"
#include    "CCACommandManager.h"
#include    "MCASearchDataPC.h"
#include 	"MCAProcessManager.h"
#include    "chatngclient.hrh"
#include	"CATouchUtils.h"

#include    <aknlists.h>
#include    <aknlistquerydialog.h>
#include    <stringloader.h>
#include	<chatNG.rsg>
#include    <csxhelp/imng.hlp.hrh>
#include    <aknenv.h>
#include    <cntdef.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include   "CAUtils.h"
#include 	"VariantKeys.h"

// CONSTANTS
const TInt KChatMaxSearchCritAmount = 4;
const TInt KGranularity = 2;
const TInt KTabSpace = 3;

// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CCASearchViewContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCASearchViewContainer* CCASearchViewContainer::NewL(
    MCASearchInterfacePC* aSearchInterfacePC,
    const TRect& aRect,
    MCAViewSwitcher& aViewSwitcher,
    MCARecipientObserver& aMenuObserver,
    TBool aSearchUsers,
    TBool aGroupSyncDone,
    const TDesC& aListId )
    {
    CCASearchViewContainer* self = new ( ELeave ) CCASearchViewContainer(
        aSearchInterfacePC,
        aViewSwitcher,
        aMenuObserver,
        aSearchUsers,
        aGroupSyncDone );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aListId );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCASearchViewContainer::CCASearchViewContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCASearchViewContainer::CCASearchViewContainer(
    MCASearchInterfacePC* aSearchInterfacePC,
    MCAViewSwitcher& aViewSwitcher,
    MCARecipientObserver& aMenuObserver,
    TBool aSearchUsers,
    TBool aGroupSyncDone )
        : iSearchInterfacePC( aSearchInterfacePC ),
        iViewSwitcher( aViewSwitcher ),
        iMenuObserver( aMenuObserver ),
        iDeleteFlag( EFalse ),
        iTaskComplete( ETrue ),
        iSearchUsers( aSearchUsers ),
        iGroupSyncDone( aGroupSyncDone )
    {
    }

// -----------------------------------------------------------------------------
// CCASearchViewContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCASearchViewContainer::ConstructL( const TRect& aRect,
                                         const TDesC& aListId )
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::ConstructL" ) );
    CreateWindowL();

    iListId = aListId.AllocL();

    iAppUi = static_cast<CCAAppUi*>( CEikonEnv::Static()->AppUi() );
    iAppUi->AddLayoutChangeObserver( this );

    // To get online/offline events

    iCommandManager
    = CCACommandManagerFactory::InstanceL()->GetCommandManager();
    MCASessionHandlerCmd* sessHandlerCmd
    = iCommandManager->GetSessionHandlerCmd() ;
    if ( sessHandlerCmd )
        {
        sessHandlerCmd->AddServiceStateObserversL( this );
        }


    iItemArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    iListBox = new ( ELeave ) CAknDoubleStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );

    iListBox->SetContainerWindowL( *this );
    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    iListBox->Model()->SetItemTextArray( iItemArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    // Get search interface
    iSearchDataPC = iAppUi->GetProcessManager().GetSearchDataInterface();

    iOnlinePresenceList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    iBlockingUI = CCABlockingUI::NewL(
                      iAppUi->GetProcessManager().GetBlockingInterface() );

    SetRect( aRect );
    ActivateL();
    // This sets correct results to navi pane
    UpdateNaviPaneTextL( ETrue );

    // Something to title
    HBufC* title = StringLoader::LoadLC( R_CHATCLIENT_SEARCH_RESULTS_TITLE,
                                         KNullDesC );
    iViewSwitcher.CAStatusPane()->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );


    // Idle loop for starting login sequence login
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    iTaskComplete = EFalse;
    iIdle->Start( TCallBack( BackgroundTasks, this ) );
    }

// -----------------------------------------------------------------------------
// CCASearchViewContainer::Destructor
// -----------------------------------------------------------------------------
//
CCASearchViewContainer::~CCASearchViewContainer()
    {
    if ( iAppUi && !iAppUi->IsUnderDestruction() )
        {
        iAppUi->RemoveLayoutChangeObserver( this );
        if ( iWaitVisible )
            {
            TInt err;
            TRAP( err, iAppUi->DismissWaitDialogL( KErrNone ) );
            }

        MCASessionHandlerCmd* sessHandlerCmd
        =  iCommandManager->GetSessionHandlerCmd() ;
        if ( sessHandlerCmd )
            {
            sessHandlerCmd->RemoveServiceStateObservers( this );
            }
        }

    iOnlineArray.Close();
    delete iOnlinePresenceList;
    delete iItemArray;
    delete iListBox;
    delete iBlockingUI;

    if ( iIdle )
        {
        iIdle->Deque();
        }
    delete iIdle;

    delete iListId;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::DisplaySearchTypeDlgL()
// Displays search type dialog with radiobuttons
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::DisplaySearchTypeDlgL()
    {
    iPreviousSearchType = iSearchType;
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::DisplaySearchTypeDlgL" ) );
    iSearchType = iSearchUsers ? ESearchUsers : ESearchGroups;
    return DisplaySearchCriteriaDlgL( iSearchType );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::DisplaySearchCriteriaDlgL()
// Displays search criteria dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::DisplaySearchCriteriaDlgL(
    const TSearchType aSearchType )
    {
    CHAT_DP( D_CHAT_LIT(
                 "CCASearchViewContainer::DisplaySearchCriteriaDlgL( %d )" ),
             aSearchType );

    TInt searchCriteria( 0 );
    TInt result( 0 );

    RArray< TEnumsPC::TSearchCriteria > indexArray; // to map return value of query to correct enum
    CleanupClosePushL( indexArray );

    CAknListQueryDialog* dlg = new ( ELeave )
    CAknListQueryDialog( &searchCriteria );
    dlg->PrepareLC( R_CHATCLIENT_SEARCH_CRITERIA );

    CDesCArray* textArray = new ( ELeave ) CDesCArrayFlat( KChatMaxSearchCritAmount );
    CleanupStack::PushL( textArray );

    switch ( aSearchType )
        {
        case ESearchUsers:
            {
            // Dynamically create the array of search criteria, according to
            // the brand variation flags
            if ( IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SEARCH_NAME ) )
                {
                HBufC* txtUserName = CCoeEnv::Static()->AllocReadResourceLC(
                                         R_QTN_CHAT_SEARCH_CRITER_USER_NAME );
                textArray->AppendL( *txtUserName );
                CleanupStack::PopAndDestroy( txtUserName );
                indexArray.AppendL( TEnumsPC::ESearchUserName );
                }

            if ( IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SEARCH_USERID ) )
                {
                HBufC* txtWVId = CCoeEnv::Static()->AllocReadResourceLC(
                                     R_QTN_CHAT_SEARCH_CRITER_USER_WV );
                textArray->AppendL( *txtWVId );
                CleanupStack::PopAndDestroy( txtWVId );
                indexArray.AppendL( TEnumsPC::ESearchUserID );
                }

            if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_PHONE ) )
                {
                HBufC* txtTelNumber = CCoeEnv::Static()->AllocReadResourceLC(
                                          R_QTN_CHAT_SEARCH_CRITER_USER_PUH );
                textArray->AppendL( *txtTelNumber  );
                CleanupStack::PopAndDestroy( txtTelNumber );
                indexArray.AppendL( TEnumsPC::ESearchUserMobileNumber );
                }

            if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SEARCH_EMAIL ) )
                {
                HBufC* txtEmail = CCoeEnv::Static()->AllocReadResourceLC(
                                      R_QTN_CHAT_SEARCH_CRITER_USER_EMAIL );
                textArray->AppendL( *txtEmail );
                CleanupStack::PopAndDestroy( txtEmail );
                indexArray.AppendL( TEnumsPC::ESearchUserEmailAddress );
                }
            break;
            }
        case ESearchGroups:
            {
            // Dynamically create the array of search criteria, according to
            // the brand variation flags
            if ( IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SEARCH_GROUPNAME ) )
                {
                HBufC* txtGroupName = CCoeEnv::Static()->AllocReadResourceLC(
                                          R_QTN_CHAT_SEARCH_CRITER_GRP_NAME );
                textArray->AppendL( *txtGroupName );
                CleanupStack::PopAndDestroy( txtGroupName );
                indexArray.AppendL( TEnumsPC::ESearchGroupName );
                }

            if ( IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SEARCH_GROUPTOPIC ) )
                {
                HBufC* txtUserWVId = CCoeEnv::Static()->AllocReadResourceLC(
                                         R_QTN_CHAT_SEARCH_CRITER_GRP_TOPIC );
                textArray->AppendL( *txtUserWVId );
                CleanupStack::PopAndDestroy( txtUserWVId );
                indexArray.AppendL( TEnumsPC::ESearchGroupTopic );
                }

            if ( IMUtils::IntResourceValueL(
                     RSC_CHAT_VARIATION_SEARCH_GROUPMEMBER ) )
                {
                HBufC* txtGroupMember = CCoeEnv::Static()->AllocReadResourceLC(
                                            R_QTN_CHAT_SEARCH_CRITER_GRP_USER );
                textArray->AppendL( *txtGroupMember );
                CleanupStack::PopAndDestroy( txtGroupMember );
                indexArray.AppendL( TEnumsPC::ESearchGroupUserIDJoined );
                }
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( textArray );
    CleanupStack::Pop( textArray );

    HBufC* heading = CCoeEnv::Static()->AllocReadResourceLC(
                         R_CHATCLIENT_SEARCH_CRITERIA_USER_HEADING );
    dlg->SetHeaderTextL( *heading );

    CleanupStack::PopAndDestroy( heading );

    result = dlg->RunLD();

    TBool returnValue( EFalse );

    if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes ) )
        {
        // map the returned index to real index
        TEnumsPC::TSearchCriteria resolvedSearchCrit = indexArray[ searchCriteria ];

        switch ( resolvedSearchCrit )
            {
            case TEnumsPC::ESearchGroupName:
            case TEnumsPC::ESearchGroupTopic:
            case TEnumsPC::ESearchGroupUserIDJoined:
            case TEnumsPC::ESearchUserID:
            case TEnumsPC::ESearchUserMobileNumber:
            case TEnumsPC::ESearchUserEmailAddress:
                {
                returnValue = DisplaySearchStringQueryL( resolvedSearchCrit );
                break;
                }
            case TEnumsPC::ESearchUserName:
                {
                CHAT_DP_TXT( "SEARCH criteria is EImpsUserFirstName" );
                returnValue = DisplayMultiLineDataQueryL( );
                break;
                }

            default:
                {
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy(); // indexArray.Close()

    return returnValue;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::DisplaySearchStringQueryL()
// Displays search string query
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::DisplaySearchStringQueryL(
    TEnumsPC::TSearchCriteria aSearchCriteria )
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::DisplaySearchStringQueryL" ) );

    HBufC* searchString = HBufC::NewLC( KMaxSearchStringLength );
    TPtr searchStringPtr( searchString->Des() );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL( searchStringPtr );
    if ( aSearchCriteria == TEnumsPC::ESearchUserMobileNumber )
        {
        dlg->PrepareLC( R_CHATCLIENT_SEARCH_MOBNMB_QUERY );
        }
    else if ( aSearchCriteria == TEnumsPC::ESearchGroupUserIDJoined
              || aSearchCriteria == TEnumsPC::ESearchUserID )
        {
        // AH neutral queries for WVID also have a special label resource
        CleanupStack::PushL( dlg );
        TBool textualInputMode = IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_USERID_TEXTUAL_INPUTMODE );
        CleanupStack::Pop( dlg );
        dlg->PrepareLC( textualInputMode ?
                        R_CHATCLIENT_SEARCH_WVID_QUERY :
                        R_CHATCLIENT_SEARCH_WVID_QUERY2 );
        }
    else if ( aSearchCriteria == TEnumsPC::ESearchUserEmailAddress )
        {
        // General queries which always demand latin characters
        // even in AH build and others.
        dlg->PrepareLC( R_CHATCLIENT_SEARCH_STRING_QUERY_AH_NEUTRAL );
        }
    else
        {
        dlg->PrepareLC( R_CHATCLIENT_SEARCH_STRING_QUERY );
        }

    // enable T9
    dlg->SetPredictiveTextInputPermitted( ETrue );

    TInt result( dlg->RunLD() );
    if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes ) )
        {
        // Build the search pairs//////////////////////////////////////////////

        RArray<TEnumsPC::TSearchCriteria>  searchCriteriaArray;
        CleanupClosePushL( searchCriteriaArray );

        CDesCArray* searchStringArray =  new ( ELeave ) CDesCArrayFlat( 1 );
        CleanupStack::PushL( searchStringArray );

        if ( searchStringPtr.Length() > 0 )
            {
            searchCriteriaArray.Append( aSearchCriteria );
            searchStringArray->AppendL( searchStringPtr );
            CCAAppUi* appUi = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );
            MCASearchInterfacePC* searchInterfacePC
            = appUi->GetProcessManager().GetSearchInterface();
            searchInterfacePC->SetRequestL( searchCriteriaArray,
                                            *searchStringArray );
            }

        CleanupStack::PopAndDestroy( searchStringArray ); //searchStringArray
        CleanupStack::PopAndDestroy( ); // searchCriteriaArray.Close()
        CleanupStack::PopAndDestroy( searchString );
        return ETrue;
        }

    CleanupStack::PopAndDestroy( searchString );

    return EFalse;
    }


// ---------------------------------------------------------
// CCASearchViewContainer::DisplayMultiLineDataQueryL()
// Displays multiline data query. First line is for firsname
// and second line is for surname.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::DisplayMultiLineDataQueryL( )
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::DisplayMultiLineDataQueryL" ) );

    HBufC* firstName = HBufC::NewLC( KMaxFirstNameLength );
    HBufC* surname = HBufC::NewLC( KMaxSurnameLength );

    HBufC* searchString = HBufC::NewLC( KMaxFirstNameLength +
                                        KMaxSurnameLength + KSpace().Length() );

    TPtr firstNamePtr( firstName->Des() );
    TPtr surnamePtr( surname->Des() );

    CCAMultiLineDataQueryDialog* dlg =
        CCAMultiLineDataQueryDialog::NewL( firstNamePtr, surnamePtr );

    dlg->PrepareLC( R_CHATCLIENT_SEARCH_NAME_QUERY );
    // enable T9
    dlg->SetPredictiveTextInputPermitted( ETrue );
    TInt result( dlg->RunLD() );

    TPtr searchStringPtr( searchString->Des() );
    searchStringPtr.Append( *firstName );
    searchStringPtr.Append( KEmptyDesC );
    searchStringPtr.Append( *surname );

    CHAT_DP_TXT( "Before query" );

    if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyYes ) )
        {
        // Build the search pairs//////////////////////////////////////////////

        RArray<TEnumsPC::TSearchCriteria>  searchCriteriaArray;
        CleanupClosePushL( searchCriteriaArray );

        CDesCArray* searchStringArray =  new ( ELeave ) CDesCArrayFlat( KGranularity );
        CleanupStack::PushL( searchStringArray );


        // Append firstname
        if ( firstNamePtr.Length() > 0 )
            {
            searchCriteriaArray.Append( TEnumsPC::ESearchUserName );
            searchStringArray->AppendL( firstNamePtr );
            }

        if ( surnamePtr.Length() > 0 )
            {
            searchCriteriaArray.Append( TEnumsPC::ESearchUserLastName );
            searchStringArray->AppendL( surnamePtr );
            }

        if ( firstNamePtr.Length() > 0 ||  surnamePtr.Length() > 0 )
            {
            CCAAppUi* appUi = static_cast <CCAAppUi*> ( CCoeEnv::Static()->AppUi() );
            MCASearchInterfacePC* searchInterfacePC
            = appUi->GetProcessManager().GetSearchInterface();
            searchInterfacePC->SetRequestL( searchCriteriaArray, *searchStringArray );
            }


        CleanupStack::PopAndDestroy( searchStringArray ); //searchStringArray
        CleanupStack::PopAndDestroy(  ); // searchCriteriaArray.Close()

        CleanupStack::PopAndDestroy( 3, firstName ); // searchString, surname, firstName
        return ETrue;
        }

    CleanupStack::PopAndDestroy( 3, firstName ); // searchString, surname, firstName
    return EFalse;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::SearchType() const
// returns searchtype
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCASearchViewContainer::TSearchType CCASearchViewContainer::SearchType() const
    {
    return iSearchType;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::SearchMoreL()
// ---------------------------------------------------------
//
void CCASearchViewContainer::SearchMoreL()
    {
    CHAT_DP_TXT( "CCASearchViewContainer::SearchMoreL" );
    //If error happens then we need old indexes
    TInt oldSearchIndex( iNextSearchIndex );
    TInt oldCurrentResultPage( iCurrentResultPage );
    iNextSearchIndex += KMaxSearchResults;
    TInt searchCount( iSearchDataPC->HowManyResultsAvailable() );
    //Next searchIndex cannot be bigger than search count
    if ( iNextSearchIndex > searchCount )
        {
        iNextSearchIndex = searchCount - 1;
        }


    ++iCurrentResultPage;
    //If currentresultpage is bigger than total result page then
    //currentresultpage is total resultpage
    if ( iCurrentResultPage > iTotalResultPages )
        {
        iCurrentResultPage = iTotalResultPages;
        }
    CHAT_DP( D_CHAT_LIT( " iCurrentResultPage = %d iTotalResultPages = %d" ),
             iCurrentResultPage, iTotalResultPages );
    CHAT_DP( D_CHAT_LIT( " iNextSearchIndex = %d " ), iNextSearchIndex );


    iAppUi->ShowWaitDialogL( R_QTN_CHAT_WNOTE_SEARCHING );
    TInt err( KErrNone );
    TRAPD( nextErr, err = iSearchInterfacePC->SearchNextL( iNextSearchIndex,
                                                           NULL ) );

    //If search fails dialog is dismissed and index are set as previous state
    if ( nextErr )
        {
        iAppUi->DismissWaitDialogL( nextErr );
        iNextSearchIndex = oldSearchIndex;
        iCurrentResultPage = oldCurrentResultPage;
        }

    //If search fails dialog is dismissed and index are set as previous state
    if ( err )
        {
        iAppUi->DismissWaitDialogL( err );
        iNextSearchIndex = oldSearchIndex;
        iCurrentResultPage = oldCurrentResultPage;
        }
    else
        {
        TInt maxDataCount( iSearchDataPC->SearchDataCount() );

        iLowIndex = iNextSearchIndex;

        iHighIndex = iLowIndex + ( KMaxSearchResults - 1 );

        if ( iHighIndex >= maxDataCount )
            {
            iHighIndex = maxDataCount - 1;
            }
        if ( iSearchType == ESearchUsers )
            {
            //If searching users and presence fetch fails dialog is dismissed
            TRAPD( fetchErr, FetchPresenceL( iLowIndex, iHighIndex ) );
            if ( fetchErr != KErrNone )
                {
                iAppUi->DismissWaitDialogL( fetchErr );
                }
            }
        //Is view update fails dialog is dismissed
        TRAPD( updateErr, UpdateListBoxL( !iSearchUsers ) );
        if ( updateErr )
            {
            iAppUi->DismissWaitDialogL( updateErr );
            }

        CHAT_DP( D_CHAT_LIT(
                     "CCASearchViewContainer::SearchMoreL HideWaitNoteL" ) );
        iAppUi->DismissWaitDialogL( KErrNone );
        }


    }

// ---------------------------------------------------------
// CCASearchViewContainer::SearchPreviousL()
// ---------------------------------------------------------
//
void CCASearchViewContainer::SearchPreviousL()
    {
    CHAT_DP_TXT( "CCASearchViewContainer::SearchPreviousL" );

    TInt oldSearchIndex( iNextSearchIndex );
    TInt oldCurrentResultPage( iCurrentResultPage );
    //Setting up search indexes
    iNextSearchIndex -= KMaxSearchResults;
    if ( iNextSearchIndex < 0 )
        {
        iNextSearchIndex = 0;
        }

    //Setting up result page numbers that are shown in navipane
    --iCurrentResultPage;
    if ( iCurrentResultPage < 1 )
        {
        iCurrentResultPage = 1;
        }
    CHAT_DP( D_CHAT_LIT( " iCurrentResultPage = %d iTotalResultPages = %d" ),
             iCurrentResultPage, iTotalResultPages );
    CHAT_DP( D_CHAT_LIT( " iNextSearchIndex = %d " ), iNextSearchIndex );

    iAppUi->ShowWaitDialogL( R_QTN_CHAT_WNOTE_SEARCHING );

    TInt err( KErrNone );
    TRAPD( nextErr, err = iSearchInterfacePC->SearchNextL( iNextSearchIndex,
                                                           NULL ) );
    if ( nextErr )
        {
        iAppUi->DismissWaitDialogL( nextErr );
        iNextSearchIndex = oldSearchIndex;
        iCurrentResultPage = oldCurrentResultPage;
        }

    if ( err )
        {
        iAppUi->DismissWaitDialogL( err );
        iNextSearchIndex = oldSearchIndex;
        iCurrentResultPage = oldCurrentResultPage;
        }
    else
        {
        TInt maxDataCount( iSearchDataPC->SearchDataCount() );

        iLowIndex = iNextSearchIndex;

        iHighIndex = iLowIndex + ( KMaxSearchResults - 1 );

        if ( iHighIndex >= maxDataCount )
            {
            iHighIndex = maxDataCount - 1;
            }

        if ( iSearchType == ESearchUsers )
            {
            TRAPD( fetchErr, FetchPresenceL( iLowIndex, iHighIndex ) );
            if ( fetchErr != KErrNone )
                {
                iAppUi->DismissWaitDialogL( fetchErr );
                }
            }
        TRAPD( updateErr, UpdateListBoxL( !iSearchUsers ) );
        if ( updateErr )
            {
            iAppUi->DismissWaitDialogL( updateErr );
            }
        CHAT_DP( D_CHAT_LIT(
                     "CCASearchViewContainer::SearchPreviousL HideWaitNoteL" ) );
        iAppUi->DismissWaitDialogL( KErrNone );
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::SetObserverToNull()
// ---------------------------------------------------------
//
void CCASearchViewContainer::SetObserverToNull()
    {
    }

// ---------------------------------------------------------
// CCASearchViewContainer::SaveAsFavouriteL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::SaveAsFavouriteL()
    {
    HBufC* groupId = SearchData().AllocLC();

    iSearchInterfacePC->CreateGroupL( *groupId );

    iAppUi->ShowWaitDialogL( R_QTN_CHAT_CLIST_WAIT_SAVING );

    TInt err( KErrNone );

    TBool hasCommonProps( EFalse );
    TEnumsPC::TImpsCSPAllErrors impsError = TEnumsPC::EImpsErrNone;
    TPtrC groupName( KNullDesC() );
    TRAPD( leave, groupName.Set( iSearchInterfacePC->GetGroupNameL(
                                     *groupId, hasCommonProps, impsError ) ) );
    if ( impsError == TEnumsPC::EImpsErrNone )
        {
        iAppUi->DismissWaitDialogL( leave );
        if ( ( err != KErrNone ) || ( !hasCommonProps ) )
            {
            // Filter out the insufficient priviledges message,
            // since it's not an error if server doesn't allow us
            // to see the name of the group.
            if ( err != ( TInt ) TEnumsPC::ECSPInsufficientGroupPrivileges )
                {
                IMNoteMapper::ShowNoteL( err );
                }
            }
        else
            {
            iSearchInterfacePC->SetGroupNameL( *groupId, groupName );
            }
        iSearchInterfacePC->SaveChangesL( *groupId );

        }
    else
        {
        iAppUi->DismissWaitDialogL( leave );
        }

    CHAT_DP_FUNC_DP( "SaveAsFavouriteL", "Group saved" );


    CleanupStack::PopAndDestroy( groupId );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::AddToFriendsL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::AddToFriendsL()
    {
    // Make sure we have contact list
    if ( CCAUINGUtils::VerifyContactlistL() == CCAUINGUtils::EChatNoContactList )
        {
        return;
        }

    HBufC* wvuidBuf = SearchData().AllocLC();

    CCAStatusPaneHandler& statusPane = *iViewSwitcher.CAStatusPane();
    HBufC* titleText = statusPane.Title()->AllocLC();

    TBool contactSaved( EFalse );

    CCAContactEditor* contactEditor = CCAContactEditor::NewL( -1,
                                                              contactSaved );

    TRAPD( err, contactEditor->EditContactLD( TEnumsPC::EAddToContact, KNullDesC, *wvuidBuf,
                                              KNullDesC ) );
    if ( err )
        {
        statusPane.SetTitleL( *titleText );
        User::Leave( err );
        }

    statusPane.SetTitleL( *titleText );

    CleanupStack::PopAndDestroy( 2, wvuidBuf ); // titleText, wvuidBuf
    }

// ---------------------------------------------------------
// CCASearchViewContainer::StartConversationL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::StartConversationL()
    {
    // Check status of contact
    if ( !IsOnline() )
        {
        // Check if user wants to start conversation with offline contact
        if ( IMUtils::IgnoreOfflineStatusL(
                 iSearchDataPC->IdentificationL( SearchData() ) ) < 0 )
            {
            return;
            }
        }

    //Add the conversation item to open chats array before switching the view
    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();
    TPtrC wvid( SearchData() );
    mainViewArrayPC->InsertConversationItemL( wvid, iSearchDataPC->IdentificationL( wvid ) );

    TCADnlConvView dnlConView;

    //collecting only first KMaxWVIDLength character in iWVID.
    dnlConView.iWVID = ( CAUtils::DisplayId( SearchData() ) ).Left( KMaxWVIDLength );
    dnlConView.iIsForwarded = EFalse;
    dnlConView.iSAPChanged = EFalse;
    dnlConView.iSwitchTab = EFalse;
    TCADnlConvViewBuf convMsgBuf( dnlConView );

    CHAT_DP( D_CHAT_LIT( "start conversation with %S" ), &dnlConView.iWVID );
    iViewSwitcher.SwitchViewL( KUidConversationsView, KUidConvViewMsgId,
                               convMsgBuf );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::InviteL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::InviteL()
    {

    /**
    * 1) PairsArray is Owned. Ownership is transferred from
    * 2) There's no need to skip groups in this case. but we do need to make sure
         the pairs are in correct order after sorting
    * 3) We can use CCAServerContactsArray, as we deal with descriptor tuple
         just as with contacts. the class naming is a bit misleading
    */
    MCAServerContactsArrayPC* pairsArray
    = iSearchInterfacePC->GetPairArrayForServerContactsArrayLC();


    TInt selectedIndex( 0 );

    // Check status of contact
    if ( !IsOnline() )
        {
        // Check if user wants to invite offline contact
        if ( IMUtils::IgnoreOfflineStatusL(
                 iSearchDataPC->IdentificationL( SearchData() ) ) < 0 )
            {
            // Stop invitation process
            CleanupStack::PopAndDestroy(); // pairsArray

            return;
            }
        }

    TInt retVal = IMDialogUtils::DisplayListQueryDialogL( &selectedIndex,
                                                          pairsArray,
                                                          R_SEARCHVIEW_SELECT_INVITE_GROUP_DLG );

    if ( retVal == EAknSoftkeyOk || retVal == EAknSoftkeyDone )
        {
        if ( selectedIndex == KErrNotFound )
            {
            User::Leave( KErrNotFound );
            }
        CCAInviteEditor* editor = CCAInviteEditor::NewL(
                                      pairsArray->WVIDArray().MdcaPoint( selectedIndex ) );

        editor->CreateAndSendInvitation(  *( iAppUi->GroupUtils() ) , SearchData() );
        delete editor;
        }

    CleanupStack::PopAndDestroy(); // pairsArray

    }

// ---------------------------------------------------------
// CCASearchViewContainer::EndSearchL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::EndSearchL()
    {
    iTotalResultPages = 0;
    iCurrentResultPage = 0;
    iNextSearchIndex = 0;
    iLowIndex = 0;
    iPreviousResultsShowing = EFalse;
    iHighIndex = 0;
    iOnlinePresenceList->Reset();
    UpdateNaviPaneTextL( ETrue );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::NewSearchL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::NewSearchL()
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::NewSearchL" ) );
    if ( DisplaySearchTypeDlgL() )
        {
        if ( iSearchInterfacePC->IsSearchPairsSet() )
            {
            StartSearchL( );
            }

        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::JoinToGroupL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::JoinToGroupL()
    {
    HBufC* groupId = SearchData().AllocLC();
    static_cast <CCAAppUi*> ( iEikonEnv->AppUi() )
    ->GroupUtils()->JoinGroupL( EFalse, *groupId );
    CleanupStack::PopAndDestroy( groupId );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::BlockUserL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::BlockUserL()
    {
    iBlockingUI->BlockUserL( SearchData() );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::IsBlocked()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::IsBlocked() const
    {
    TPtrC searchedData ( SearchData() );
    MCABlockingPC* blockingPC = iAppUi->GetProcessManager().GetBlockingInterface();
    return blockingPC->IsContactBlocked( searchedData );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::ListBox()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CEikListBox* CCASearchViewContainer::ListBox() const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::SearchData()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCASearchViewContainer::SearchData() const
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::SearchData" ) );
    TInt curIndex = iListBox->CurrentItemIndex();
    TInt dataCount = iSearchDataPC->SearchDataCount();
    if ( curIndex < 0 || dataCount == 0 )
        {
        return KNullDesC();
        }
    else
        {
        TInt searchDataIndex( curIndex + ( ( iCurrentResultPage - 1 ) *
                                           KMaxSearchResults ) );
        if ( searchDataIndex >= dataCount )
            {
            searchDataIndex = dataCount - 1;
            }
        if ( searchDataIndex < 0 )
            {
            searchDataIndex = 0;
            }
        return iSearchDataPC->SearchData( searchDataIndex );
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::IsOnline()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::IsOnline() const
    {
    TInt curIndex( iListBox->CurrentItemIndex() );

    if ( ( curIndex < 0 ) || ( curIndex >= iOnlineArray.Count() ) )
        {
        return EFalse;
        }
    return iOnlineArray[ curIndex ];
    }

// ---------------------------------------------------------
// CCASearchViewContainer::BlockUserWVIdL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::BlockUserWVIdL()
    {
    iBlockingUI->DisplayBlockTextQueryL();
    }
// ---------------------------------------------------------
// CCASearchViewContainer::UnBlockL()
// ---------------------------------------------------------
void CCASearchViewContainer::UnBlockL()
    {
    iBlockingUI->DisplayUnblockListQueryL();
    }
// ---------------------------------------------------------
// CCASearchViewContainer::DisplayBlockedListL()
// ---------------------------------------------------------
//
void CCASearchViewContainer::DisplayBlockedListL()
    {
    iBlockingUI->DisplayBlockedListL( ETrue );
    UpdateNaviPaneTextL( EFalse );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::MoreResultsAvailableL() const
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::MoreResultsAvailable() const
    {
    return ( iCurrentResultPage < iTotalResultPages );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::PrevResultsAvailableL() const
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::PrevResultsAvailable() const
    {
    return ( iCurrentResultPage != 1 );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::IsFavouriteChatGroupL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCASearchViewContainer::IsFavouriteChatGroup() const
    {
    TBool isFavourite = iSearchInterfacePC->IsFavouriteChatGroup( SearchData() );
    return isFavourite;
    }


// ---------------------------------------------------------
// CCASearchViewContainer::HandleError()
// ---------------------------------------------------------
//
void CCASearchViewContainer::HandleSearchError( TInt aErrorCode )
    {
    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::HandleError( %d )" ),
             aErrorCode );
    // CodeScanner warning ignored because CS does not regognize TRAP being used
    // after line break
    TRAPD( err, ( iAppUi->DismissWaitDialogL( aErrorCode ) ) );  // CSI: 42 # See comment above
    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }


// ---------------------------------------------------------
// CCASearchViewContainer::HandleSearchFinished()
// ---------------------------------------------------------
//
void CCASearchViewContainer::HandleSearchFinished( )
    {
    }

// ---------------------------------------------------------
// CCASearchViewContainer::StartSearchL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::StartSearchL( )
    {
    // Construct string for status pane title from aSearchPairs

    HBufC* searchString = iSearchInterfacePC->GetSearchStringLC();

    iViewSwitcher.CAStatusPane()->SetTitleL(
        R_CHATCLIENT_SEARCH_RESULTS_TITLE, *searchString );

    CleanupStack::PopAndDestroy( searchString );

    // Check iSearchDataPC->SearchDataCount() > 0
    // it is more reliable because app engine keeps previous results


    CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::StartSearchL" ) );
    if ( iPreviousResultsShowing || iSearchDataPC->SearchDataCount() > 0 )
        {
        ClearListBoxContents();

        iAppUi->ShowWaitDialogL( R_QTN_CHAT_WNOTE_SEARCHING );
        TRAPD( endErr, EndSearchL() );
        if ( endErr )
            {
            iAppUi->DismissWaitDialogL( endErr );
            User::Leave( endErr );
            }
        }
    else
        {
        iAppUi->ShowWaitDialogL( R_QTN_CHAT_WNOTE_SEARCHING );
        }

    TInt err( KErrNone );
    TRAPD( startErr, err = iSearchInterfacePC->StartSearchL(
                               KMaxSearchResults, NULL , NULL ) );

    if ( startErr )
        {
        iAppUi->DismissWaitDialogL( startErr );
        User::Leave( startErr );
        }

    if ( err )
        {
        CHAT_DP( D_CHAT_LIT( "CCASearchViewContainer::StartSearchL err=%d " ), err );

        if ( err == TEnumsPC::ECSPNotSupported || err == TEnumsPC::ECSPInvalidParameter )
            {
            // map 402 and 405 to "search criteria not supported"
            iAppUi->DismissWaitDialogL( KErrNone );
            IMDialogUtils::DisplayInformationNoteL(
                R_QTN_CHAT_SEARCH_NOT_SUPPORTED );
            }
        else
            {
            iAppUi->DismissWaitDialogL( err );
            }
        }
    else
        {
        iPreviousResultsShowing = ETrue;

        TInt maxDataCount( iSearchDataPC->SearchDataCount() );
        CHAT_DP( D_CHAT_LIT(
                     "CCASearchViewContainer::StartSearchL maxDataCount = %d " ),
                 maxDataCount );

        if ( maxDataCount > 0 )
            {
            iCurrentResultPage = 1;
            iLowIndex = 0;

            iHighIndex = iLowIndex + ( KMaxSearchResults - 1 ); //-1 because of array

            if ( iHighIndex >= maxDataCount )
                {
                iHighIndex = maxDataCount - 1;
                }

            if ( iSearchType == ESearchUsers )
                {
                // Presence fetching failed => not a big problem
                TRAPD( fetchErr, FetchPresenceL( iLowIndex, iHighIndex ) );
                if ( fetchErr != KErrNone )
                    {
                    CHAT_DP( D_CHAT_LIT(
                                 "CCASearchViewContainer::StartSearchL FetchPresenceL() err=%d " ),
                             fetchErr );
                    iAppUi->DismissWaitDialogL( fetchErr );
                    User::Leave( fetchErr );
                    }
                }

            CHAT_DP( D_CHAT_LIT(
                         "CCASearchViewContainer::StartSearchL UpdateListBoxL() " ) );
            TRAPD( updateErr, UpdateListBoxL(
                       iSearchType == ESearchUsers ? EFalse : ETrue ) );
            if ( updateErr )
                {
                iAppUi->DismissWaitDialogL( updateErr );
                User::Leave( updateErr );
                }

            CHAT_DP( D_CHAT_LIT(
                         "CCASearchViewContainer::StartSearchL HideWaitNoteL" ) );
            iAppUi->DismissWaitDialogL( KErrNone );
            }
        else
            {
            // no search results
            iAppUi->DismissWaitDialogL( KErrNone );

            }
        }
    }

// ---------------------------------------------------------------------------
// CCASearchViewContainer::SetEmptyTextsToListboxL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CCASearchViewContainer::SetEmptyTextsToListboxL()
    {
    HBufC* emptyFinal = IMUtils::CombineStringFromResourceLC
                        ( R_QTN_CHAT_EMPTY_SEARCH_PRIMARY,
                          R_QTN_CHAT_EMPTY_SEARCH );

    iListBox->View()->SetListEmptyTextL( *emptyFinal );
    CleanupStack::PopAndDestroy(); // emptyFinal
    }


// ---------------------------------------------------------
// CCASearchViewContainer::HandleListBoxEventL
// Handles list box events
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                  TListBoxEvent aEventType )
    {
    if ( aEventType == EEventItemDoubleClicked )
        {
        iMenuObserver.ShowPopUpMenuL();
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::SizeChanged()
// Called by framework
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::CountComponentControls()
// Returns number of controls
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCASearchViewContainer::CountComponentControls() const
    {
    //Only one compound control so returning 1
    return 1;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::ComponentControl( TInt aIndex )
// Returns control for given index
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCoeControl* CCASearchViewContainer::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iListBox;
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::OfferKeyEventL()
// Handles user key inputs
// (other items were commented in a header).
// ---------------------------------------------------------
//
TKeyResponse CCASearchViewContainer::OfferKeyEventL( const TKeyEvent& aEvent,
                                                     TEventCode aType )
    {
    if ( aEvent.iCode == EKeyEnter || aEvent.iCode == EKeyOK )
        {
        iMenuObserver.ShowPopUpMenuL();
        return EKeyWasConsumed;
        }
    return iListBox->OfferKeyEventL( aEvent, aType );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::GetHelpContext
// This function is called when Help application is launched
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidChatClient;
    aContext.iContext = KIMNG_HLP_SEARCHCHAT ;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::FocusChanged
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::FocusChanged( TDrawNow /* aDrawNow */ )
    {
    if ( iListBox )
        {
        // give focus to list box so that highlight animations
        // are done properly
        iListBox->SetFocus( IsFocused() );
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::FetchPresenceL()
// ---------------------------------------------------------
//
void CCASearchViewContainer::FetchPresenceL( const TInt aLowIndex,
                                             const TInt aHighIndex )
    {

    CHAT_DP_TXT( "CCASearchViewContainer::FetchPresenceL" );
    CHAT_DP( D_CHAT_LIT( "   aLowIndex %d aHighIndex %d" ), aLowIndex,
             aHighIndex );



    // Calculate optimal granularity, which is from low index to high index.
    TInt gran = aHighIndex - aLowIndex + 1;
    // Creating array with granularity 0 crashes, make sure gran is > 0
    if ( gran == 0 )
        {
        gran = 1;
        }
    CDesCArrayFlat* userIDs = new ( ELeave ) CDesCArrayFlat( gran );
    CleanupStack::PushL( userIDs );

    for ( TInt arrayIndex( aLowIndex ); arrayIndex <= aHighIndex;
          ++arrayIndex )
        {
        userIDs->AppendL( iSearchDataPC->SearchData( arrayIndex ) );
        }

    CHAT_DP_TXT( "Getting online users..." );

    MCAMainViewArrayPC* mainViewArrayPC =
        iAppUi->GetProcessManager().GetArrayInterface();
    mainViewArrayPC->GetOnlineUsersL( userIDs, iOnlinePresenceList, NULL );



    CHAT_DP_TXT( "...got online users" );

    CleanupStack::PopAndDestroy( userIDs );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::UpdateNaviPaneTextL()
// ---------------------------------------------------------
//
void CCASearchViewContainer::UpdateNaviPaneTextL( TBool aEndSearch )
    {
    // Else this is zero
    if ( !aEndSearch )
        {
        iTotalResultPages = ( iSearchDataPC->HowManyResultsAvailable() /
                              KMaxSearchResults );
        if ( ( iSearchDataPC->HowManyResultsAvailable() %
               KMaxSearchResults ) != 0 )
            {
            ++iTotalResultPages; // 1 / 5 = 0 -> add 1
            }

        if ( iSearchDataPC->HowManyResultsAvailable() > 0 &&
             iTotalResultPages == 0 )
            {
            iTotalResultPages = 1;
            }
        }
    // Only two items in array, so 2 should be enough

    CArrayFix<TInt>* intArray = new ( ELeave ) CArrayFixFlat<TInt>( KGranularity );
    CleanupStack::PushL( intArray );
    intArray->AppendL( iCurrentResultPage );
    intArray->AppendL( iTotalResultPages );
    HBufC* naviText = StringLoader::LoadLC( R_CHATCLIENT_SEARCH_NAVI,
                                            *intArray );
    TPtrC naviTextPtr( naviText->Des() );
    CHAT_DP( D_CHAT_LIT( "*setting navi pane text to %S" ), &naviTextPtr );
    iViewSwitcher.CAStatusPane()->SetNaviLabelL( naviTextPtr );
    CleanupStack::PopAndDestroy( 2, intArray ); // naviText, intArray
    }

// ---------------------------------------------------------
// CCASearchViewContainer::UpdateListBoxL()
// ---------------------------------------------------------
//
void CCASearchViewContainer::UpdateListBoxL( TBool aGroup )
    {

    TInt granularity( iItemArray->Count() );

    if ( granularity == 0 )
        {
        ++granularity;
        }

    CDesCArray* tempArray = new ( ELeave ) CDesCArrayFlat( granularity );
    CleanupStack::PushL( tempArray );
    TInt loopCount( iItemArray->Count() );
    for ( TInt i( 0 ); i < loopCount; ++i )
        {
        tempArray->AppendL( iItemArray->MdcaPoint( i ) );
        }
    ClearListBoxContents();
    iListBox->Model()->SetItemTextArray( tempArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iListBox->HandleItemAdditionL();

    TInt arrayIndex( iLowIndex );
    HBufC* online = iEikonEnv->AllocReadResourceLC(
                        R_SEARCHVIEW_USER_ONLINE );

    HBufC* offline = iEikonEnv->AllocReadResourceLC(
                         R_SEARCHVIEW_USER_OFFLINE );
    do
        {
        CHAT_DP( D_CHAT_LIT( "Update list box..." ) );

        if ( aGroup )
            {
            TPtrC wvid( iSearchDataPC->SearchData( arrayIndex ) );
            TBool tempGroup( EFalse );
            TBool foundGroup = iSearchInterfacePC->FindGroup( wvid );
            if ( !foundGroup )
                {
                // we must create a temporary group for groupmanager
                // Ownership not transferred to caller
                iSearchInterfacePC->CreateGroupL( wvid ); // CSI: 35 # See comment above
                tempGroup = ETrue;
                }

            // now we have the group so we can fetch the properties for it
            TBool hasCommonProps( EFalse );
            TEnumsPC::TImpsCSPAllErrors impsError = TEnumsPC::EImpsErrNone;
            TPtrC groupName =  iSearchInterfacePC->GetGroupNameL( wvid, hasCommonProps, impsError );
            TPtrC firstLine( KEmptyDesC );
            TPtrC secondLine( KEmptyDesC );

            // if properties are not get, then set only wvid
            if ( impsError == TEnumsPC::EImpsErrNone && hasCommonProps )
                {
                firstLine.Set( groupName );
                secondLine.Set( CCAAppUi::DisplayId(
                                    iSearchDataPC->SearchData( arrayIndex ) ) );
                }
            else
                {
                firstLine.Set( CCAAppUi::DisplayId(
                                   iSearchDataPC->SearchData( arrayIndex ) ) );
                }

            // Construct list item text
            HBufC* groupText = HBufC::NewLC( firstLine.Length() +
                                             secondLine.Length() +
                                             KTabSpace ); // 3 for tabulators
            TPtr groupTextPtr( groupText->Des() );
            groupTextPtr.Append( KTabulator );
            groupTextPtr.Append( firstLine );
            groupTextPtr.Append( KTabulator );
            groupTextPtr.Append( secondLine );
            groupTextPtr.Append( KTabulator );

            // now we can delete our temporary group (do not delete
            // from server) we can ignore the error as it can only
            // come from server
            if ( tempGroup )
                {
                iSearchInterfacePC->DeleteGroupL( wvid, EFalse );
                }

            iItemArray->AppendL( groupTextPtr );
            CleanupStack::PopAndDestroy( groupText );
            }
        else
            {
            HBufC* itemText = HBufC::NewLC( KMaxFirstNameLength +
                                            KMaxSurnameLength );
            TPtr itemTextPtr( itemText->Des() );

            itemTextPtr.Append( KTabulator );

            TPtrC searchedData = iSearchDataPC->SearchData( arrayIndex );
            itemTextPtr.Append( iSearchDataPC->IdentificationL( searchedData ) );
            itemTextPtr.Append( KTabulator );

            // Now we should get the presence of this guy
            TInt ignoreIndex( 0 );
            // We found online presence
            if ( iOnlinePresenceList->Find( searchedData, ignoreIndex ) == 0 )
                {
                itemTextPtr.Append( *online );
                iOnlineArray.Append( ETrue );
                }
            else
                {
                itemTextPtr.Append( *offline );
                iOnlineArray.Append( EFalse );
                }

            itemTextPtr.Append( KTabulator );
            iItemArray->AppendL( itemTextPtr );
            CleanupStack::PopAndDestroy( itemText );
            }

        ++arrayIndex;
        } while ( arrayIndex <= iHighIndex );

    CleanupStack::PopAndDestroy( 3, tempArray ); // offline, online, itemText
    iListBox->Model()->SetItemTextArray( iItemArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    CHAT_DP( D_CHAT_LIT(
                 "CCASearchViewContainer::UpdateListBoxL Item array count %d" ),
             iItemArray->Count() );
    CHAT_DP( D_CHAT_LIT(
                 "CCASearchViewContainer::UpdateListBoxL Reseting online presence list" ) );
    iOnlinePresenceList->Reset();

    iListBox->HandleItemAdditionL();
    UpdateNaviPaneTextL( EFalse );
    if ( iItemArray->MdcaCount() > 0 )
        {
        iListBox->SetCurrentItemIndexAndDraw( 0 );
        }
    iListBox->UpdateScrollBarsL();

    }

// ---------------------------------------------------------
// CCASearchViewContainer::SetGroupSyncDone
// ---------------------------------------------------------
//
void CCASearchViewContainer::SetGroupSyncDone()
    {
    CHAT_DP_TXT( "CCASearchViewContainer::SetGroupSyncDone - Group sync done" );
    iGroupSyncDone = ETrue;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::ClearListBoxContents()
// ---------------------------------------------------------
//
void CCASearchViewContainer::ClearListBoxContents()
    {
    iItemArray->Reset();
    iOnlineArray.Reset();
    TRAPD( err, iListBox->HandleItemRemovalL() );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    iListBox->DrawNow();
    }

// -----------------------------------------------------------------------------
// CCAAppUi::BackgroundTasks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASearchViewContainer::BackgroundTasks( TAny *aInstance )
    {
    return static_cast<CCASearchViewContainer*>( aInstance )->DoBackgroundTasks();
    }

// -----------------------------------------------------------------------------
// CCAAppUi::DoBackgroundTasks
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCASearchViewContainer::DoBackgroundTasks()
    {
    TBool retVal( EFalse );
    TRAPD( err, retVal = ContainerStartL() );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }

    // if the task is completed...
    if ( !retVal )
        {
        if ( iDeleteFlag )
            {
            // delete self and cancel callback
            delete this;
            return EFalse;
            }

        iTaskComplete = ETrue;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCASearchViewContainer::ContainerStartL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCASearchViewContainer::ContainerStartL()
    {
    iSearchType = iSearchUsers ? ESearchUsers : ESearchGroups;


    // if groups are not supported, just show the search type dialog and that's
    // it. DON'T show any notes.
    TBool groupsSupported = iAppUi->UISessionManager().IsSupported(
                                CCAUISessionManager::EGroup );

    if ( ! groupsSupported )
        {
        if ( iSearchInterfacePC->IsSearchPairsSet() )
            {
            StartSearchL( );
            }
        else
            {
            NewSearchL();
            }
        return EFalse;  // that's it, one-shot
        }

    if ( ! iWaitVisible && ! iGroupSyncDone )
        {
        CHAT_DP_TXT(
            "CCASearchViewContainer::ContainerStartL - ! iWaitVisible && ! iGroupSyncDone" );
        // launch the wait note
        iWaitVisible = ETrue;

        iAppUi->ShowWaitDialogL( R_QTN_CHAT_SEARCHVIEW_PROCESSING, ETrue );

        return ETrue;
        }

    if ( iGroupSyncDone )
        {
        CHAT_DP_TXT( "CCASearchViewContainer::ContainerStartL - iGroupSyncDone" );

        // hide wait note
        if ( iWaitVisible )
            {
            CHAT_DP_TXT( "CCASearchViewContainer::ContainerStartL - iWaitVisible" );

            TRAPD( err, iAppUi->DismissWaitDialogL( KErrNone ) );
            if ( err != KErrNone )
                {
                CActiveScheduler::Current()->Error( err );
                }

            // group has been synched
            iWaitVisible = EFalse;
            }

        CHAT_DP_TXT( "CCASearchViewContainer::ContainerStartL - No more wait note, show search type dialog" );
        // that's it, out of here

        if ( iSearchInterfacePC->IsSearchPairsSet() )
            {
            StartSearchL( );
            }
        else
            {
            NewSearchL();
            }
        return EFalse;
        }

    // nothing interesting, do nothing, but come back here
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCASearchViewContainer::ListId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCASearchViewContainer::ListId() const
    {
    if ( iListId )
        {
        return *iListId;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------
// CCASearchViewContainer::DeleteSelfD
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::DeleteSelfD()
    {
    iDeleteFlag = ETrue;

    // if task is already completed -> delete immediately
    if ( iTaskComplete )
        {
        delete this;
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::LayoutChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::LayoutChangedL( TInt aType )
    {
    // updates tabgroup
    iViewSwitcher.CAStatusPane()->ShowTabGroupL( KUidSearchView );

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    SetRect( mainPaneRect );
    UpdateNaviPaneTextL( ETrue );
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CCASearchViewContainer::HandleServiceStateChangeL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCASearchViewContainer::HandleServiceStateChangeL( TServiceState aState )
    {
    if ( aState == EOffline )
        {
        // Clear search results if we got disconnected.
        EndSearchL();
        ClearListBoxContents();
        iGroupSyncDone = EFalse;
        }
    }

// ---------------------------------------------------------
// CCASearchViewContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CCASearchViewContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( !iItemArray->Count() == 0 )
        {
        iListBox->HandlePointerEventL( aPointerEvent );
        return;
        }


    if ( !CATouchUtils::PenEnabled() )
        {
        return;
        }
    // Single tap on main pane (not on scroll bar)
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        iMenuObserver.ShowPopUpMenuL();
        }
    }
//  End of File
