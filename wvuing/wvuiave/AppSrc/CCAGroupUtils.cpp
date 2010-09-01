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
* Description:  Group handling utilities
*
*/


//INCLUDES
#include    "ImpsCSPAllErrors.h"
#include    "CCAGroupUtils.h"

#include    "chatngclient.hrh"
#include    "ChatDefinitions.h"
#include    "CAExternalInterface.h"
#include    "CCAAppUi.h"

#include    "ChatDebugPrint.h"
#include    "CCAGroupPropertiesDialog.h"
#include    "IMNoteMapper.h"
#include    "IMDialogUtils.h"
#include    "StringLoader.h"
#include    "CCAStatusPaneHandler.h"
#include    "CCAChatInfoArray.h"
#include    "CAArrayUtils.h"
#include    "CCAUISessionManager.h"
#include    "IMUtils.h"
#include    "CAUtils.h"
#include    "CCAApp.h"
#include    "CCAVariantFactory.h"
#include    "CCAContactSelectionDialog.h"
#include    "CCAUINGUtils.h"
#include    "CCAPCUtils.h"

#include    "MCAConversationMessage.h"
#include	"MCAProcessManager.h"
#include    "MCAGroupPC.h"
#include 	"MCAGroupUtilsPC.h"


#include    <chatNG.rsg>
#include    <chatNG.mbg>
#include    <impsgroupprops.h>
#include    <aknnotewrappers.h>
#include    <aknlists.h>
#include    <bautils.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

// -----------------------------------------------------------------------------
// CCAGroupUtils::CCAGroupUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAGroupUtils::CCAGroupUtils( MCAViewSwitcher& aViewSwitcher )
        : iViewSwitcher( aViewSwitcher ),
        iCurrState( ESGCNone ),
        iPrevState( ESGCNone ),
        iScreenNamePtr( NULL, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAGroupUtils* CCAGroupUtils::NewLC( MCAViewSwitcher& aViewSwitcher )
    {
    CCAGroupUtils* temp = new( ELeave )CCAGroupUtils( aViewSwitcher );
    CleanupStack::PushL( temp );
    temp->ConstructL();
    return temp;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::ResolveInviteConflictL
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::ResolveInviteConflictL( const TDesC& aGroup,
                                            CDesCArray& aArray )
    {





    // get the banned and all user list  from pc
    TInt countInvite( aArray.MdcaCount() );

    if ( !countInvite )
        {
        //no possible conflicts
        return;
        }

    iAppUi->ShowWaitDialogLC( R_QTN_GEN_CHAT_NOTE_PROCESSING, ETrue );

    TInt err( KErrNone );


    CDesCArray* banned = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( banned );

    // Check server capabilities for group reject list
    if ( iAppUi->UISessionManager().IsSupported( CCAUISessionManager::EGroupRejectList ) )
        {
        // If supported, fetch list
        err = iGroupUtilsPC->GetRejectListL( *banned, aGroup );
        }


    if ( err == ECSPInsufficientGroupPrivileges )
        {
        //not enough group rights
        CleanupStack::PopAndDestroy( 2 ); // banned, wait
        return;
        }

    User::LeaveIfError( err );

    if ( banned->MdcaCount() == 0 )
        {
        //no possible conflict , nothing to do, just cleanup
        CleanupStack::PopAndDestroy( 2 ); // banned, wait
        return;
        }


    CDesCArray* userList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( userList );
    CDesCArray* screenNames  = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( screenNames );
    CDesCArray* moders = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( moders );
    CDesCArray* admins = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( admins );

    err = iGroupUtilsPC->GetMembersL( *userList,
                                      *screenNames,
                                      *moders,
                                      *admins, aGroup );
    User::LeaveIfError( err );

    CDesCArray* newBanned = CAArrayUtils::CloneArrayLC( *banned );
    CDesCArray* newUserlist = CAArrayUtils::CloneArrayLC( *userList );

    TInt bannedCount( banned->MdcaCount() );
    TInt pos( KErrNotFound );
    TInt userlistInUse = userList->MdcaCount();

    for ( TInt i( 0 ); i < bannedCount; i++ )
        {
        pos = CAUtils::NeutralFind( aArray, banned->MdcaPoint( i ) );
        if ( pos != KErrNotFound )
            {
            //we have a conflict to resolve
            HBufC* prompt = StringLoader::LoadLC( R_QTN_CHAT_INVITE_REMOVE_BANNED,
                                                  iGroupUtilsPC->GetIdentificationL( banned->MdcaPoint( i ) ) );

            TInt ret( IMDialogUtils::DisplayQueryDialogL( R_GENERIC_YES_NO_CONFIRMATION_QUERY,
                                                          *prompt ) );
            CleanupStack::PopAndDestroy( prompt );

            if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes )  )
                {
                //lets remove from the banned list
                newBanned->Delete( i );
                if ( userlistInUse )
                    {
                    newUserlist->AppendL( banned->MdcaPoint( i ) );
                    }
                }
            else
                {
                //lets removed from the invitation list
                aArray.Delete( pos );
                }
            }
        }

    if ( newBanned->MdcaCount() != bannedCount )
        {
        //we need to update group props
        CDesCArray* newAdminlist = CAArrayUtils::CloneArrayLC( *admins );
        iGroupUtilsPC->UpdateGroupMembersL(
            aGroup,
            *admins, *newAdminlist,
            *userList, *newUserlist,
            *banned, *newBanned );
        CleanupStack::PopAndDestroy( newAdminlist );
        }

    // newUserlist , newBanned, admins, moders, screenNames
    CleanupStack::PopAndDestroy( 8 ); // banned, userlist, wait
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::~CCAGroupUtils
// Destructor
// -----------------------------------------------------------------------------
//
CCAGroupUtils::~CCAGroupUtils()
    {
    delete iGroupId;
    delete iAsyncCb;
    delete iJoinGroupId;

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    delete iAsyncGroupId;
    delete iAsyncScreenName;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::ConstructL()
    {
    iAppUi = static_cast<CCAAppUi*>( CEikonEnv::Static()->AppUi() );
    iGroupPC = iAppUi->GetProcessManager().GetGroupInterface();
    iGroupUtilsPC = iAppUi->GetProcessManager().GetGroupUtilsInterface() ;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::EditChatGroupPropertiesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::EditChatGroupPropertiesL( const TDesC& aGroupId )
    {


    CHAT_DP_FUNC_ENTER( "EditChatGroupPropertiesL" );

    TBool areItemsEdited( EFalse );

    // create lists
    CDesCArray* userList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( userList );
    CDesCArray* screenNames  = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( screenNames );
    CDesCArray* moders = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( moders );
    CDesCArray* admins = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( admins );
    CDesCArray* rejected = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( rejected );

    TInt err( KErrNone );
    iAppUi->ShowWaitDialogL( R_QTN_GEN_CHAT_NOTE_PROCESSING, ETrue );


    TRAPD( leave, err = iGroupUtilsPC->GetGroupDataL( aGroupId, *userList, *screenNames,
                                                      *moders, *admins, *rejected ) );

    iAppUi->DismissWaitDialogL( leave );

    if ( err != KErrNone )
        {
        IMNoteMapper::ShowNoteL( err );
        CleanupStack::PopAndDestroy( 5, userList ); // rejected, admins,
        // moders, screenNames, userList
        return EAknSoftkeyCancel;
        }

    iGroupUtilsPC->MoveOwnIdToFirstL( *admins );

    // store the original arrays
    // (we'll ignore screennames and moderators)
    CDesCArray* oldUserList = CAArrayUtils::CloneArrayLC( *userList );
    CDesCArray* oldAdmins = CAArrayUtils::CloneArrayLC( *admins );
    CDesCArray* oldRejected = CAArrayUtils::CloneArrayLC( *rejected );


    // construct the groupproperties dialog
    CCAGroupPropertiesDialog* groupPropDialog =
        new( ELeave )CCAGroupPropertiesDialog(
        *iViewSwitcher.CAStatusPane(),
        ETrue,
        *userList,
        *admins,
        *rejected,
        aGroupId,
        areItemsEdited );

    CleanupStack::PushL( groupPropDialog );
    groupPropDialog->ConstructL( R_CHATCLIENT_MENUBAR_CHAT_PROPERTIES_VIEW );

    // launch the dialog
    CleanupStack::Pop( groupPropDialog );
    TInt result( groupPropDialog->ExecuteLD( R_CHATCLIENT_GROUPPROPERTIES_DLG ) );
    CHAT_DP( D_CHAT_LIT(
                 "CCAGroupUtils::EditChatGroupPropertiesL areItemsEdited: %d" ),
             areItemsEdited );

    if ( ( result == EAknSoftkeyDone || result == EAknSoftkeyYes ) && areItemsEdited )
        {
        // Display wait note. Move this to utils side.
        iAppUi->ShowWaitDialogL( R_QTN_GEN_NOTE_SAVING, ETrue );

        // Update properties
        TInt err( KErrNone );

        TRAP( leave, err = iGroupUtilsPC->ProcessEditedGroupPropertiesL(
                               aGroupId, *oldAdmins, *admins, *oldUserList, *userList,
                               *oldRejected, *rejected ) );

        iAppUi->DismissWaitDialogL( leave );
        if ( err != KErrNone )
            {
            IMNoteMapper::ShowNoteL( err );
            CleanupStack::PopAndDestroy( 8, userList );
            return EAknSoftkeyCancel;
            }
        }
    else if ( result == EAknSoftkeyExit )
        {
        CEikonEnv::Static()->EikAppUi()->HandleCommandL( EChatClientCmdExit );
        }

    CleanupStack::PopAndDestroy( 8, userList );
    CHAT_DP_FUNC_DONE( "EditChatGroupPropertiesL" );
    return result;
    }

// utilsside_complete
// -----------------------------------------------------------------------------
// CCAGroupUtils::AskScreenNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::AskScreenNameL( TDes& aScreenName )
    {
    // don't call this directly, call ScreenNameQueryL instead

    // make sure we're not capturing
    TBool wasCapturing = iAppUi->AreEventsCaptured();
    iAppUi->ReleaseCapturingL();

    // ask for the screenname
    TInt result = IMDialogUtils::DisplayTextQueryDialogL( aScreenName,
                                                          R_CHATCLIENT_CLIST_SN_PROMPT,
                                                          R_CHATCLIENT_CLIST_SN_QUERY,
                                                          ETrue // T9
                                                        );

    if ( wasCapturing )
        {
        // restore capturing
        iAppUi->CaptureEventsL();
        }

    return result;
    }
// pcside_partially ..impact has to be analysed


// -----------------------------------------------------------------------------
// CCAGroupUtils::CreateNewChatL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAGroupUtils::CreateNewChatL( TInt aResourceId, const TUid aViewId,
                                      TBool aContactFound, TEnumsPC::TListSelectionType aSelType,
                                      TInt aSelectedListIndex   )
    {
    CHAT_DP_FUNC_ENTER( "CreateNewChatL" );

    if ( !iAppUi->UISessionManager().IsLoggedIn() )
        {
        iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );
        if ( !iAppUi->UISessionManager().IsLoggedIn() )
            {
            // Still not logged in
            // Can't continue
            return NULL;
            }
        }

    if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_SIMPLE_GROUP_CREATION ) )
        {
        // simplified new school group creation
        CHAT_DP_FUNC_DONE( "CreateNewChatL" );
        return CreateNewChatSimpleL( aResourceId, aViewId , aContactFound, aSelType, /*aOnlineContacts*/ aSelectedListIndex );
        }
    else
        {
        // normal old school group creation
        CHAT_DP_FUNC_DONE( "CreateNewChatL" );
        return CreateNewChatNormalL( aResourceId, aViewId );
        }
    }
//pcside_partially

// -----------------------------------------------------------------------------
// CCAGroupUtils::CreateNewChatNormalL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAGroupUtils::CreateNewChatNormalL(
    TInt aResourceId,
    const TUid aViewId )
    {
    CHAT_DP_FUNC_ENTER( "CreateNewChatNormalL" );

    TBool areItemsEdited( EFalse );

    // create lists
    CDesCArray* userList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( userList );
    CDesCArray* admins = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( admins );
    CDesCArray* rejected = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( rejected );

    // add own wvid to admins list
    // Use new function for this


    iGroupUtilsPC->MoveOwnIdToFirstL( *admins );

    // note: groupprops must be at the top of the cleanupstack
    // so that we can get them out before call to creategroupl
    CImpsCommonGroupProps* groupProps = CImpsCommonGroupProps::NewL();
    CleanupStack::PushL( groupProps );
    CImpsPrivateGroupProps* privateProps = CImpsPrivateGroupProps::NewL();
    CleanupStack::PushL( privateProps );

    CImpsCommonGroupProps* newGroupProps = CImpsCommonGroupProps::NewL();
    CleanupStack::PushL( newGroupProps );
    CImpsPrivateGroupProps* newPrivateProps = CImpsPrivateGroupProps::NewL();
    CleanupStack::PushL( newPrivateProps );

    // construct and launch the groupproperties dialog
    CCAGroupPropertiesDialog* groupPropDialog =
        new( ELeave )CCAGroupPropertiesDialog(
        *iViewSwitcher.CAStatusPane(),
        /**groupProps, *privateProps, *newGroupProps, *newPrivateProps,*/

        EFalse,
        *userList, *admins, *rejected,
        KNullDesC,
        areItemsEdited );

    CleanupStack::PushL( groupPropDialog );
    groupPropDialog->ConstructL( R_CHATCLIENT_MENUBAR_CHAT_PROPERTIES_VIEW );
    CleanupStack::Pop( groupPropDialog );
    TInt result( groupPropDialog->ExecuteLD(
                     R_CHATCLIENT_GROUPPROPERTIES_DLG ) );
    CHAT_DP( D_CHAT_LIT(
                 "CCAGroupUtils::CreateNewChatNormalL areItemsEdited: %d" ),
             areItemsEdited );

    HBufC* groupId = NULL;

    if ( result == EAknSoftkeyDone || result == EAknSoftkeyYes )
        {
        CCAUINGUtils::SetTitleL( aResourceId, aViewId );

        // ask screenname query before creation, or re-use alias
        TBool joinGroup( EFalse );

        // fetch the default or recently used screen name pointer
        iScreenNamePtr.Set( iAppUi->ScreenNamePtr()->Des() );
        TInt ret = ScreenNameQueryL( iScreenNamePtr );

        if ( ret == EAknSoftkeyDone || ret == EAknSoftkeyOk )
            {
            CDesCArray* noteTexts = new( ELeave ) CDesCArrayFlat(
                KArrayGranularity );
            CleanupStack::PushL( noteTexts );
            noteTexts->AppendL( newGroupProps->GroupName() );
            noteTexts->AppendL( iScreenNamePtr );
            HBufC* waitMsg = StringLoader::LoadLC(
                                 RSC_CHAT_JOIN_WAIT, *noteTexts );
            iAppUi->ShowWaitDialogL( *waitMsg, ETrue );
            CleanupStack::PopAndDestroy( 2, noteTexts ); // waitMsg, noteTexts
            joinGroup = ETrue;
            }
        else
            {
            iAppUi->ShowWaitDialogL( R_QTN_GEN_CHAT_NOTE_PROCESSING, ETrue );
            }

        // check the autodeletion variation flag
        if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_AUTODELETE ) )
            {
            newGroupProps->SetAutoDelete( EImpsPropYes );
            }

        // CreateGroupL takes the ownership of the properties
        CleanupStack::Pop( 2, newGroupProps ); // newPrivateProps, newGroupProps
        CleanupStack::PopAndDestroy( 2, groupProps ); // privateProps, groupProps

        TInt error( 0 );
        TBool whisperingAllowed =
            IMUtils::IntResourceValueL(
                RSC_CHAT_VARIATION_GROUP_WHISPER ) == 1;
        TRAPD( err, groupId = iGroupUtilsPC->CreateGroupSimpleL( iScreenNamePtr,
                                                                 joinGroup,
                                                                 whisperingAllowed,
                                                                 error ) );

        // if CreateGroupL returns NULL, it couldn't create the group
        if ( groupId )
            {
            // Delete old one. No need to store it.
            // Cannot put groupId to CleanupStack because if PushL leaves then
            // wait note would be shown forever
            delete iGroupId;
            iGroupId = groupId;
            }

        CHAT_DP( D_CHAT_LIT(
                     "CCAGroupUtils::CreateNewChatNormalL()  iGroupInterface->CreateGroupL() error = %d, err = %d" ),
                 error, err );

        if ( err == KErrNone && error == KErrNone && groupId )
            {
            TRAPD( leave, iGroupUtilsPC->AddGroupMembersL( *groupId, *admins, *userList, *rejected ) );
            iAppUi->DismissWaitDialogL( leave );

            if ( joinGroup )
                {
                // switch to joined group (ETrue = ask invite query)
                SwitchToJoinedGroupL( EFalse, *groupId, NULL, ETrue );
                }
            }
        // Engine couldn't find a unique group id
        else if ( err == KErrNone && error == KErrNone && ! groupId )
            {
            // Imps_ERROR_BASE - 10000 is an unknown error
            // ("server error") to note mapper
            iAppUi->DismissWaitDialogL( Imps_ERROR_BASE - 10000 );
            }
        else
            {
            iAppUi->DismissWaitDialogL( err );
            if ( error == ECSPAccepted )
                {
                CCAUINGUtils::HandlePartialSuccessL();
                }
            else
                {
                IMNoteMapper::ShowNoteL( error );
                if ( error == ECSPMaxNumberOfGroupsForUser ||
                     error == ECSPMaxNumberOfGroupsForServer )
                    {
                    DisplayCreatedChatRoomsListL();
                    }
                }
            }
        }
    else
        {
        // user cancelled the creation
        CleanupStack::PopAndDestroy( 4, groupProps ); //newPrivateProps,
        // newGroupProps, privateProps, groupProps
        CCAUINGUtils::SetTitleL( aResourceId, aViewId );
        if ( result == EAknSoftkeyExit )
            {
            CEikonEnv::Static()->EikAppUi()->HandleCommandL(
                EChatClientCmdExit );
            }
        }
    CleanupStack::PopAndDestroy( 3, userList ); // rejected, admins, userList

    if ( groupId )
        {
        // groupId points to iGroupId and the ownership is
        // passed to calling function so we can't delete the iGroupId
        iGroupId = NULL;
        }
    // no leaving code allowed between (iGroupId = NULL;) and (return groupId;)
    CHAT_DP_FUNC_DONE( "CreateNewChatNormalL" );

    return groupId;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::CreateNewChatSimpleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAGroupUtils::CreateNewChatSimpleL( TInt /* aResourceId */,
                                            const TUid /* aViewId */, TBool aContactFound,
                                            TEnumsPC::TListSelectionType aSelType,
                                            TInt aSelectedListIndex )
    {
    CHAT_DP_FUNC_ENTER( "CreateNewChatSimpleL" );

    CDesCArray* inviteList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( inviteList );

    HBufC* groupId = NULL;
    HBufC* groupName = HBufC::NewLC( KMaxChatGroupLenght );
    HBufC* inviteMsg = HBufC::NewLC( KInviteMessageMaxLength );
    TPtr chatGroupName( groupName->Des() );
    TPtr chatInvite( inviteMsg->Des() );
    TPtr screenName( iAppUi->ScreenNamePtr()->Des() );
    TBool inviteSupported( iAppUi->UISessionManager().IsSupported( CCAUISessionManager::EInvite ) );



    TInt result = EAknSoftkeyOk;
    TInt inviteError = KErrNone;

    // number of online friends
    //TInt onlineCount = 0;

    iCurrState = ESGCInviteContacts;
    iPrevState = ESGCNone;

    TBool exitFSM = EFalse;

    //   MAKE SURE EACH STATE HAS CLEANUPSTACK BALANCED UPON EXIT
    //   This means: DO NOT CleanupStack::Push in one state, and
    //   Pop/PopAndDestroy in the other. This also means to do Pop/PopAndDestroy
    //   before a state transition inside a state.

    // handle simplified group creation within a finite state machine
    // construction



    while ( ! exitFSM )
        {
        switch ( iCurrState )
            {
                // BEGIN state: choosing the contacts to be invited
            case ESGCInviteContacts :
                {
                // we only ask for invite if invite function is supported
                result = EAknSoftkeyOk;


                if ( aContactFound  )
                    {


                    // We have online friends, so show the multiselection view
                    CCAAppUi* appUi = static_cast<CCAAppUi*>(
                                          CCoeEnv::Static()->AppUi() );


                    MCASkinVariant* skinVar = static_cast<CCAApp*>(
                                                  appUi->Application() )->VariantFactory()->SkinVariantL();
                    CDesCArrayFlat* selectedContacts =
                        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
                    CleanupStack::PushL( selectedContacts );

                    // Show selection dialog

                    TBool isSelected( EFalse );
                    // show selection dialog

                    isSelected = CCAContactSelectionDialog::ShowDialogL(
                                     *selectedContacts,
                                     *appUi->GetProcessManager().GetArrayInterface(),
                                     *skinVar,
                                     *appUi->GetProcessManager().GetSettingsInterface(),
                                     appUi->MbmFullPath(),
                                     aSelType,
                                     R_CONTACT_SELECTION_DIALOG,
                                     *iViewSwitcher.CAStatusPane(),
                                     NULL,
                                     TEnumsPC::EFilterNotDefined,
                                     TEnumsPC::EFilterNotDefined,
                                     EFalse,
                                     aSelectedListIndex
                                     , &exitFSM );
                    if ( exitFSM )
                        {
                        CleanupStack::PopAndDestroy();
                        break;
                        }

                    if ( isSelected )
                        {
                        // code merge 14 Nov, 2006
                        // Get rid of contacts with duplicate ids
                        CCAPCUtils::RemoveDuplicateWVIds( *selectedContacts );
                        // Populate "real" invite list
                        TInt inviteCount( selectedContacts->MdcaCount() );

                        for ( TInt i = 0; i < inviteCount; ++i )
                            {

                            TPtrC wvid( selectedContacts->MdcaPoint( i ) );

                            inviteList->AppendL( wvid );
                            }

                        // Continue creation if user selected invitees
                        if ( inviteList->Count() > 0 )
                            {
                            result = EAknSoftkeyOk;
                            }
                        else
                            {
                            result = EAknSoftkeyCancel;
                            }


                        }
                    else
                        {
                        result = EAknSoftkeyCancel;
                        }

                    CleanupStack::PopAndDestroy(); // selectedContacts.Close()
                    }


                StateChange( result, KErrNone, ESGCGroupName );
                break;
                }

            // ask the group name
            case ESGCGroupName :
                {
                if ( aSelType == TEnumsPC::ESingleListMultiSelect )
                    {
                    MCAMainViewArrayPC* mainViewArrayPC =
                        iAppUi->GetProcessManager().GetArrayInterface();
                    chatGroupName = mainViewArrayPC->DisplayName( aSelectedListIndex );
                    }
                iGroupUtilsPC->GetDefaultGroupNameL( chatGroupName, aSelType );
                // since the user didn't cancel, ask the group name
                result = IMDialogUtils::DisplayTextQueryDialogL( chatGroupName,
                                                                 R_CHATCLIENT_CHATLIST_CREATE_GROUP_NAME,
                                                                 R_CHATCLIENT_CHATLIST_CREATE_GROUP_NAME_QUERY,
                                                                 ETrue // T9
                                                               );

                StateChange( result, KErrNone, ESGCInviteMessage );
                break;
                }

            // ask the invitation message
            case ESGCInviteMessage :
                {
                if ( inviteList->Count() > 0 )
                    {
                    // the user didn't cancel, so ask the invitation message
                    result = IMDialogUtils::DisplayTextQueryDialogL(
                                 chatInvite,
                                 R_CHAT_INVITE_MSG_DQ,
                                 R_CHAT_INVITE_MESSAGE_QUERY,
                                 ETrue // T9
                             );

                    StateChange( result, KErrNone, ESGCScreenName );
                    }
                else
                    {
                    // nobody in invite list, force state change
                    StateChange( EAknSoftkeyOk, KErrNone, ESGCScreenName );
                    }
                break;
                }

            // ask the screen name (or use alias)
            case ESGCScreenName :
                {
                result = ScreenNameQueryL( screenName );
                StateChange( result, KErrNone, ESGCCreateGroup );
                break;
                }

            // do the actual creation, sending of invitations,
            // and joining to the group
            case ESGCCreateGroup :
                {
                // finalize chat group creation and join it, then send all the
                // invitations
                // put up the wait note
                // wait note is dismissed by the last state
                HBufC* waitMsg = StringLoader::LoadLC(
                                     R_CHAT_CREATING_CHAT_GROUP );
                iAppUi->ShowWaitDialogL( *waitMsg, ETrue );
                CleanupStack::PopAndDestroy(); // waitMsg

                TInt err;
                TInt error = 0;
                TRAP( err, groupId = iGroupUtilsPC->CreateGroupL( screenName, chatGroupName, error ) );


                // CreateGroupL leaves with KErrDiskFull under low flash
                // memory conditions (below critical level, 128 kB)
                // stop group creation here in that case
                if ( err == KErrDiskFull )
                    {
                    // Use StateChange to advance to error state
                    StateChange( EAknSoftkeyOk, err, ESGCNone );
                    break;
                    }

                // if CreateGroupL returns NULL, it couldn't create the group
                if ( groupId )
                    {
                    // Delete old one. No need to store it.
                    // Cannot put groupId to CleanupStack because if PushL leaves then
                    // wait note would be shown forever
                    delete iGroupId;
                    iGroupId = groupId;
                    }

                // note, the ESGCDoJoin state and ESGCError states dismiss the
                // wait note.  ESGCDoSendInvite makes a new wait note, and it
                // gets dismissed upon successful state change to ESGCDoJoin

                // error contains the actual nw error now
                StateChange( EAknSoftkeyOk, error, ESGCDoSendInvite );
                break;
                }

            // send the invitation(s),
            case ESGCDoSendInvite :
                {
                TInt retval = KErrNone;

                if ( inviteList->MdcaCount() > 0 )
                    {
                    // we have invitations, so send them

                    // Show wait-note. Error state dismisses it.
                    iAppUi->ShowWaitDialogL( inviteList->MdcaCount() > 1 ?
                                             R_QTN_CHAT_INVITE_SENDING_MANY : R_QTN_CHAT_INVITE_SENDING,
                                             ETrue );

                    // Send invitation
                    TRAP( retval, iGroupUtilsPC->SendInvitationsL( *inviteList, *groupId, *inviteMsg ); );
                    }

                // Ignore these errors at least for now and continue group creation.
                // UI CR coming for 3.1
                if ( retval == ECSPPartiallySuccessful || retval == ECSPCannotDeliver
                     || retval == ECSPRecipientBlockedSender )
                    {
                    inviteError = retval;
                    retval = KErrNone;
                    }

                StateChange( EAknSoftkeyOk, retval, ESGCDoJoin );
                break;
                }

            // and joining to the group
            case ESGCDoJoin:
                {
                // join to group (groupId contains ID), then
                // switch to joined group
                TInt leave;
                TInt err = KErrNone;
                TRAP( leave, err = iGroupUtilsPC->JoinGroupL( *groupId, screenName, ETrue, ETrue ) );

                // Get rid of wait-note. It's either
                // "Creating chat group" or "Sending invitation"
                // at this point
                iAppUi->DismissWaitDialogL( err );

                // If there was an error in sending invitation show error note
                // at this point right before view switching
                if ( inviteError != KErrNone &&  inviteError != ECSPRecipientBlockedSender )
                    {
                    IMNoteMapper::ShowNoteL( inviteError );
                    }
                if ( inviteError == ECSPRecipientBlockedSender )
                    {
                    TInt inviteeCount = inviteList->MdcaCount();
                    for ( TInt cnt = 0; cnt < inviteeCount; cnt++ )
                        {
                        IMNoteMapper::ShowNoteL( inviteError,
                                                 CCAPCUtils::DisplayId( inviteList->MdcaPoint( cnt )  ) );
                        }
                    }
                // screen name cannot be in use since we just created the group...
                switch ( err )
                    {
                    case ECSPSuccessful:    // flowthrough
                    case KErrNone:
                        {
                        // ok, switch to joined group
                        SwitchToJoinedGroupL( EFalse, *groupId );
                        break;
                        }

                    default:
                        {
                        // will force state change to error state
                        StateChange( EAknSoftkeyOk, err, ESGCDoJoin );
                        break;
                        }
                    }

                // final state, all done.
                exitFSM = ETrue;
                break;
                }

            // got some network or other error during the process
            // not all states will end up here. only ESGCCreateGroup and
            // ESGCInviteAndJoin.
            case ESGCError :
                {
                // safe to call even without a wait dialog active
                iAppUi->DismissWaitDialogL( iNwError );

                // must be the last
                exitFSM = ETrue;

                if ( iNwError == ECSPMaxNumberOfGroupsForUser ||
                     iNwError == ECSPMaxNumberOfGroupsForServer )
                    {
                    if ( DisplayCreatedChatRoomsListL() == EAknSoftkeyOk )
                        {
                        exitFSM = EFalse; // Continue creation.
                        StateChange( EAknSoftkeyOk, KErrNone, ESGCCreateGroup );
                        }
                    }
                break;
                }

            // cancel group creation query
            case ESGCQueryCancel :
                {
                result = IMDialogUtils::DisplayQueryDialogL(
                             R_CHAT_GENERIC_CANCEL_CONFIRMATION_QUERY,
                             R_QTN_CHAT_CANCEL_GROUP_CREATION );

                if ( ( result == EAknSoftkeyOk ) ||
                        ( result == EAknSoftkeyYes ) )
                    {
                    // user really wants to cancel, so
                    // let's get out of here
                    exitFSM = ETrue;
                    }
                else if( !result )
                	{
                	if( !iAppUi->GetIsFSWExitFlag() )
                		{
                	    StateChange( EAknSoftkeyOk, KErrNone, iPrevState );
                		}
                	else
                		{
                		exitFSM = ETrue;
                		}
                	}
                else
                    {
                    // user cancelled the cancel query, so go back to the state
                    // we were in
                    iCurrState = iPrevState;
                    }
                break;
                }

            default :
                {
                // WRONG state, leave
                User::Leave( KErrArgument );
                break;
                }
            }
        }

    iCurrState = ESGCNone;
    iPrevState = ESGCNone;

    // if the user cancels in any step, we end up here
    // and do cleanup. also, if we're successful
    CleanupStack::PopAndDestroy( inviteMsg );
    CleanupStack::PopAndDestroy( groupName );
    CleanupStack::PopAndDestroy( inviteList );

    CHAT_DP_FUNC_DONE( "CreateNewChatSimpleL" );
    return NULL;
    }
// pcside_completely

// pcside_completely
// -----------------------------------------------------------------------------
// CCAGroupUtils::ScreenNameQueryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::ScreenNameQueryL( TDes& aScreenName )
    {
    TInt askScreenName = IMUtils::IntResourceValueL(
                             RSC_CHAT_VARIATION_SCREENNAME_QUERY );

    TInt result = 0;


    iGroupUtilsPC->ScreenNameQueryL( aScreenName );

    if ( ! askScreenName )
        {
        // assume as accepted as the query dialog is not to be shown
        result = EAknSoftkeyOk;
        }
    else
        {
        result = AskScreenNameL( aScreenName );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::PrepareToSwitchBackL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::PrepareToSwitchBackL( const TDesC& aGroupID,
                                          TUid aPrevView,
                                          TUid aNextView )
    {
    TUid messageID;

    HBufC8* myMessage = NULL;

    TCADnlChatViewBuf chatMsgBuf;
    TCADnlConvViewBuf convMsgBuf;


    TCADnlChatView dnlChatView;
    if ( aPrevView == KUidChatView )
        {
        TBool foundGroup = iGroupUtilsPC->FindGroup( aGroupID );

        dnlChatView.iGroupId =
            aGroupID.Left( dnlChatView.iGroupId.MaxLength() );
        dnlChatView.iIsForwarded = EFalse;
        dnlChatView.iInviteQuery = EFalse;
        if ( foundGroup )
            {
            dnlChatView.iGroupName = iGroupUtilsPC->GetEngineDefaultGroupName( aGroupID );
            }

        chatMsgBuf = dnlChatView;
        messageID = KUidChatViewMsgId;
        myMessage = chatMsgBuf.AllocLC();
        }
    else // aPrevView == KUidConversationsView
        {
        TCADnlConvView dnlConView;
        dnlConView.iWVID = aGroupID;
        dnlConView.iIsForwarded = EFalse;
        dnlConView.iSAPChanged = EFalse;
        dnlConView.iSwitchTab = EFalse;

        convMsgBuf = dnlConView;
        messageID = KUidConvViewMsgId;
        myMessage = convMsgBuf.AllocLC();
        }


    CleanupStack::Pop( myMessage );

    //myMessage ownership is taken by RegisterSwitchBack
    iAppUi->RegisterSwitchBack( aPrevView, messageID, myMessage, aNextView );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::StateChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::StateChange( TInt aAknResult, TInt aErrorCode,
                                 TSGCState aNewState )
    {
    iPrevState = iCurrState;

    if ( aErrorCode != KErrNone )
        {
        // got an error, go to error state and handle it
        iCurrState = ESGCError;
        iNwError = aErrorCode;
        return;
        }

    // new state for FSM, changes FSM state
    if ( ( aAknResult != EAknSoftkeyOk ) && ( aAknResult != EAknSoftkeyYes )
         && ( aAknResult != EAknSoftkeyDone ) )
        {
        // user cancelled the query in a step, verify
        // with a query
        iCurrState = ESGCQueryCancel;
        }
    else
        {
        // user didn't cancel, so move to a new state
        iCurrState = aNewState;
        }
    }

// Callback for asynchronous joining to the group
// after the login has been completed
TInt JoinGroupCallBack( TAny* aPtr )
    {
    return static_cast< CCAGroupUtils* >( aPtr )->DoAsyncJoin();
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::DoAsyncJoin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::DoAsyncJoin()
    {
    CHAT_DP_FUNC_ENTER( "JoinGroupCallBack" );

    TRAP( iAsyncLeave, iErr = JoinGroupL(
                                  EFalse,
                                  *iAsyncGroupId,
                                  *iAsyncScreenName,
                                  iAsyncFwdMessage ) );

    CHAT_DP_FUNC_DONE( "JoinGroupCallBack" );

    // don't call again
    return EFalse;
    }

// utilsside_complete
// -----------------------------------------------------------------------------
// CCAGroupUtils::JoinGroupL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::JoinGroupL( const TBool aFromFwd,
                                const TDesC& aGroupId,
                                const TDesC& aScreenName /*= KNullDesC*/,
                                const MCAConversationMessage* aFwdMessage /*= NULL*/,
                                TJoinMode aJoinMode /*= EStoredId*/ )
    {
    CHAT_DP_FUNC_ENTER( "JoinGroupL" );
    if( iJoinGroupId )
        {
        delete iJoinGroupId;   
        iJoinGroupId = NULL;
        }
    iJoinGroupId = aGroupId.Alloc();
    // Create copy of the forwarded message.
    // This method does not copy again the message if it's the one we have
    // already copied. aFwdMessage should not be used after this copy
    // operation, since it's not assured to stay in memory until we need it.
    // Use 'message' instead.
    iAppUi->SetForwardMessageL( aFwdMessage );
    const MCAConversationMessage* message = iAppUi->ForwardMessage();

    if ( !iAppUi->UISessionManager().IsLoggedIn() )
        {
        // not logged in, try to log in
        iAppUi->UISessionManager().LoginL( MCAUiLoginCmdCB::ELogin );

        TInt err( KErrNone );

        if ( iAppUi->UISessionManager().IsLoggedIn() )
            {
            // start the joining when we have time, i.e.
            // when the ongoing login "background tasks" have been
            // finished

            // prevent keypresses from being handled, otherwise we might crash
            // because of view change in between everything or something like
            // that
            CHAT_DP_TXT(
                "CCAGroupUtils::JoinGroupL - Start capturing key events" );
            iAppUi->CaptureEventsL();

            iAsyncGroupId = aGroupId.AllocL();
            iAsyncScreenName = aScreenName.AllocL();
            iAsyncFwdMessage = message;

            CAsyncCallBack* temp = new ( ELeave ) CAsyncCallBack(
                TCallBack( JoinGroupCallBack, this ),
                CActive::EPriorityIdle );

            delete iAsyncCb;
            iAsyncCb = temp;

            // let it rock
            iAsyncCb->CallBack();

            // wait for the request to stop
            if ( !iWait.IsStarted() )
                {
                iWait.Start();  // CSI: 10 # iWait is not an active object
                }

            err = iErr;

            CHAT_DP_TXT(
                "CCAGroupUtils::JoinGroupL - JoinGroupCallBack finished" );

            // leave with stuff the JoinGroupL left with,
            // but return the stuff it returned...
            User::LeaveIfError( iAsyncLeave );

            iAppUi->ReleaseCapturingL();
            CHAT_DP_TXT( "CCAGroupUtils::JoinGroupL - Released capturing" );
            }

        return err;
        }

    if ( !iAppUi->UISessionManager().IsSupported( CCAUISessionManager::EGroup ) )
        {
        // groups not supported, show note
        IMDialogUtils::DisplayNoteDialogL( R_CHAT_ERROR_NOT_SUP_FUNCTIONS );
        return KErrNotSupported;
        }

    CHAT_DP_TXT( "CCAGroupUtils::JoinGroupL - Beginning to join to the group" );

    TBool isGroupJoined( EFalse );
    TBool isFavourite( EFalse );

    iGroupUtilsPC->CreateOrJoinGroupL( aGroupId, isGroupJoined, isFavourite );



    // we know whether group exists or not by this time..


    TInt err( KErrNone );
    if ( isGroupJoined )
        {
        // if it's already joined, not much to do
        SwitchToJoinedGroupL( aFromFwd, aGroupId, message );
        }
    else
        {
        TInt result( 0 );
        HBufC* screenName = NULL;

        // Select correct screen name:

        if ( aScreenName.Length() > 0 )
            {
            // if the screen name query has already been done
            screenName = aScreenName.AllocLC();
            result = EAknSoftkeyOk;
            }
        else
            {
            // fetch the last used or default screen name
            TPtr screenNamePtr( iAppUi->ScreenNamePtr()->Des() );

            // confirm the screen name
            result = ScreenNameQueryL( screenNamePtr );
            screenName = screenNamePtr.AllocLC();
            }


        if ( ( result == EAknSoftkeyOk ) || ( result == EAknSoftkeyDone )  )
            {
            // screen name ok
            CDesCArray* noteTexts =
                new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
            CleanupStack::PushL( noteTexts );

            const TDesC& groupName = iGroupUtilsPC->GetEngineDefaultGroupName( aGroupId );
            if ( groupName.Length() == 0 )
                {
                // no group name, use Group ID
                noteTexts->AppendL( CCAPCUtils::DisplayId( aGroupId ) );
                }
            else
                {
                noteTexts->AppendL( groupName );
                }
            noteTexts->AppendL( *screenName );
            HBufC* waitMsg = StringLoader::LoadLC( RSC_CHAT_JOIN_WAIT, *noteTexts );

            iAppUi->ShowWaitDialogL( *waitMsg, ETrue );

            TRAPD( leave, err = iGroupUtilsPC->JoinGroupL( aGroupId,
                                                           *screenName, ETrue, isFavourite ) );

            iAppUi->DismissWaitDialogL( leave );
            if (err == EOperationCancelled)
                {
                if( iJoinGroupId )
                    {
                    delete iJoinGroupId;   
                    iJoinGroupId = NULL;
                    }
                CleanupStack::PopAndDestroy( 2);// waitMsg, noteTexts
                CleanupStack::PopAndDestroy( screenName );                                   
                return err;
                }

            CHAT_DP( D_CHAT_LIT( "CCAGroupUtils - JoinGroupL returned %d" ),
                     err );

            switch ( err )
                {
                case ECSPSuccessful:    // flowthrough
                case KErrNone:
                    {
                    if ( iGroupUtilsPC->GetEngineDefaultGroupName( aGroupId ).Length() == 0 )
                        {
                        iGroupUtilsPC->SetGroupNameL( aGroupId );
                        }

                    //Switch to joined group
                    SwitchToJoinedGroupL( aFromFwd, aGroupId, message );
                    break;
                    }
                case ECSPInvalidParameter:  // flowthrough
                case ECSPGroupDoesNotExist:
                    {
                    // the user might have manually entered a bogus group id

                    iGroupUtilsPC->DeleteFailedGroupL( aGroupId, EFalse );
                    if ( aJoinMode == EManualId )
                        {
                        // This way error can be recognized in note mapper
                        err += Imps_ERROR_BASE;
                        }

                    IMNoteMapper::ShowNoteL( err );

                    if ( aJoinMode == EManualId )
                        {
                        // Map back to original error code
                        err -= Imps_ERROR_BASE;
                        }
                    break;
                    }
                case ECSPScreennameInUse:
                    {
                    IMNoteMapper::ShowNoteL( err );
                    // Screen name was already in use, using KNullDesC
                    // as screen name so that method understands to ask
                    // screen name again
                    err = JoinGroupL( aFromFwd, aGroupId, KNullDesC, message );
                    break;
                    }
                default:
                    {

                    iGroupUtilsPC->DeleteFailedGroupL( aGroupId, EFalse );
                    IMNoteMapper::ShowNoteL( err );
                    break;
                    }
                }

            CleanupStack::PopAndDestroy( 2 ); // waitMsg, noteTexts
            }
        else
            {
            // screen name not ok
            iGroupUtilsPC->DeleteFailedGroupL( aGroupId, EFalse );

            err = EOperationCancelled;
            }

        CleanupStack::PopAndDestroy( screenName );
        }
        if( iJoinGroupId )
            {
            delete iJoinGroupId;   
            iJoinGroupId = NULL;
            }
    return err;
    }


// -----------------------------------------------------------------------------
// CCAGroupUtils::SwitchToJoinedGroupL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::SwitchToJoinedGroupL( const TBool aFromFwd,
                                          const TDesC& aGroupID,
                                          const MCAConversationMessage* aFwdMessage /*= Null*/,
                                          TBool aInviteQuery /*= EFalse*/ )
    {
    CHAT_DP_FUNC_ENTER( "SwitchToJoinedGroupL" );



    TCADnlChatViewBuf viewBuf;

    TInt err = iGroupUtilsPC->SwitchToJoinedGroupL( aGroupID, viewBuf , aFwdMessage, aInviteQuery );


    if ( err != KErrNone )
        {
        CCAUINGUtils::DisplayErrorNoteL( err );
        return;
        }

    if ( aFromFwd )
        {
        //if the join group is called from conversation view's fwdtogroup option
        //then the viewSwitcher neednt save the last view
        //and when u give back key it neednt move to the last view of conversation
        //hence we use ActivateLocalViewL
        iAppUi->ActivateLocalViewL( KUidChatView, KUidChatViewMsgId, viewBuf );
        }
    else
        {
        iViewSwitcher.SwitchViewL( KUidChatView, KUidChatViewMsgId, viewBuf );
        }
    }


// -----------------------------------------------------------------------------
// CCAGroupUtils::DisplayCreatedChatRoomsListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::DisplayCreatedChatRoomsListL()
    {


    CArrayFixFlat<TInt>* indexArray =
        new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );
    CleanupStack::PushL( indexArray );

    CDesCArray* groupIds = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( groupIds );

    CDesCArray* tempArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( tempArray );



    TInt err = iGroupUtilsPC->GetCreatedChatRoomListL( *tempArray, *groupIds );

    TInt result( IMDialogUtils::DisplayListQueryDialogL( indexArray, tempArray,
                                                         R_CHAT_CREATED_GROUPS_QUERY ) );

    CleanupStack::PopAndDestroy( tempArray );

    if ( result == EAknSoftkeyOk )
        {
        iAppUi->ShowWaitDialogLC( R_QTN_GEN_CHAT_NOTE_PROCESSING );

        TInt error( KErrNone );
        TInt selectionCount( indexArray->Count() );
        for ( TInt i( 0 ); ( i < selectionCount ) && ( error == KErrNone ); i++ )
            {
            TPtrC groupId( groupIds->MdcaPoint( indexArray->At( i ) ) );
            iGroupUtilsPC->DeleteChatGroupFromNetworkL( groupId, ETrue );
            }

        CleanupStack::Pop( 1 ); // wait dialog
        iAppUi->DismissWaitDialogL( error );
        }
    else
        {
        HBufC* errorText = StringLoader::LoadLC( R_QTN_CHAT_GROUP_CREATION_CANCELLED );
        IMDialogUtils::DisplayErrorNoteL( *errorText );
        CleanupStack::PopAndDestroy( errorText );
        }
    CleanupStack::PopAndDestroy( 2, indexArray ); // groupIds, indexArray
    return result;
    }

// pcside_partially

// -----------------------------------------------------------------------------
// CCAGroupUtils::DisplayChatInfoL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::DisplayChatInfoL( const TDesC& aGroupId )
    {
    //Let's activate wait dialog
    iAppUi->ShowWaitDialogLC( R_QTN_GEN_CHAT_NOTE_PROCESSING, ETrue );



    // create listbox and popuplist
    CAknSingleHeadingPopupMenuStyleListBox* listBox = new ( ELeave )
    CAknSingleHeadingPopupMenuStyleListBox();
    CleanupStack::PushL( listBox );

    CAknPopupList* popUpList = CAknPopupList::NewL( listBox,
                                                    R_AVKON_SOFTKEYS_OK_EMPTY, AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popUpList );

    //Constructing popuplist
    listBox->ConstructL( popUpList,
                         CEikListBox::ELeftDownInViewRect | CEikListBox::EDisableHighlight );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    TInt err = KErrNone;
    CCAChatInfoArray* itemArray = NULL;

    TRAP( err, itemArray = iGroupUtilsPC->GetChatInfoArrayL( aGroupId ) );

    if ( !itemArray ) //NOTE: dismiss dialog error could be wrong, check completely
        {

        CleanupStack::PopAndDestroy( 2,  listBox );	// listBox, popUpList
        iAppUi->DismissWaitDialogL( err );
        CleanupStack::Pop( 1 ); // wait dialog

        return;
        }

    CleanupStack::PushL( itemArray );

    // set the item array as listbox model
    CTextListBoxModel* model = listBox->Model();
    CleanupStack::Pop( itemArray );
    model->SetItemTextArray( itemArray );
    model->SetOwnershipType( ELbmOwnsItemArray );

    // set the title
    HBufC* prompt = StringLoader::LoadLC(
                        R_CHATLIST_VIEW_INFO_TITLE,
                        iGroupUtilsPC->GetEngineDefaultGroupName( aGroupId ) );

    popUpList->SetTitleL( *prompt );
    CleanupStack::PopAndDestroy( prompt );


    // hide the waitnote and execute the popuplist
    iAppUi->DismissWaitDialogL( KErrNone );

    popUpList->ExecuteLD();
    CleanupStack::Pop( popUpList );

    CleanupStack::PopAndDestroy( );	// listBox

    CleanupStack::Pop( 1 ); // wait dialog
    }





// -----------------------------------------------------------------------------
// CCAGroupUtils::IsJoinedToGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtils::IsJoinedToGroup( const TDesC& aGroupId )
    {
    return iGroupUtilsPC->IsJoined( aGroupId );
    }
// -----------------------------------------------------------------------------
// CCAGroupUtils::IsOwnGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtils::IsOwnGroup( const TDesC& aGroupId ) const
    {
    return iGroupUtilsPC->IsOwnGroup( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::IsFavouriteChatGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtils::IsFavouriteChatGroup( const TDesC& aGroupId )
    {
    return iGroupUtilsPC->IsFavouriteChatGroup( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::IsOwnGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtils::SaveAsFavouriteL( const TDesC& aGroupId )
    {

    iAppUi->ShowWaitDialogL( R_QTN_GEN_NOTE_SAVING );

    TRAPD( err, iGroupUtilsPC->SaveAsFavouriteL( aGroupId ) );

    iAppUi->DismissWaitDialogL( err );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::FindGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtils::FindGroup( const TDesC& aGroupId ) const
    {
    return iGroupUtilsPC->FindGroup( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::GetGroupName
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CCAGroupUtils::GetGroupName( const TDesC& aGroupId ) const
    {
    return iGroupUtilsPC->GetEngineDefaultGroupName( aGroupId );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::DeleteGroupL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::DeleteGroupL( const TDesC& aGroupId )
    {

    TInt retVal( KErrNone );

    HBufC* groupName = iGroupUtilsPC->GetEngineDefaultGroupName( aGroupId ).AllocLC();

    HBufC* prompt = StringLoader::LoadLC( R_DELETE_CHATGROUP_PROMPT,
                                          *groupName );

    // Show confirmation

    TInt ret( IMDialogUtils::DisplayQueryDialogL( R_GENERIC_YES_NO_CONFIRMATION_QUERY,
                                                  *prompt ) );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::PopAndDestroy( groupName );

    if ( ( ret == EAknSoftkeyOk ) || ( ret == EAknSoftkeyYes ) )
        {
        // if group is own and deletion is allowed
        // and autodeletion is not used => delete also from network

        // if group is own and deletion is allowed
        // and autodeletion is not used => delete also from network
        TBool deleteFromNetwork( IsOwnGroup( aGroupId ) &&
                                 IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_ALLOW_EDIT_GROUP_PROPERTIES )
                                 && !IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_AUTODELETE ) );

        iAppUi->ShowWaitDialogL( R_QTN_FLDR_DELETING_WAIT_NOTE );
        TRAPD( err, retVal = iGroupUtilsPC->DeleteGroupL( aGroupId, deleteFromNetwork ) );
        iAppUi->DismissWaitDialogL( err );

        if ( retVal != KErrNone )
            {
            IMNoteMapper::ShowNoteL( retVal );
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CCAGroupUtils::GetParticipantsL()
// ---------------------------------------------------------
//
void CCAGroupUtils::GetParticipantsL( CDesCArray& aParticipantList,
                                      const TDesC& aGroupID ) const
    {
    iGroupUtilsPC->GetParticipantsL( aParticipantList, aGroupID );
    }
// -----------------------------------------------------------------------------
// CCAGroupUtils::LeaveGroupL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtils::LeaveGroupL( const TDesC& aGroupId )
    {

    iAppUi->ShowWaitDialogL( R_QTN_CHAT_GROUP_LEAVING );


    TRAPD( err, iGroupUtilsPC->LeaveGroupL( aGroupId ) );

    iAppUi->DismissWaitDialogL( err );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CCASingleListContainer::DisplayJoinedMembersL()
// Displays joined members
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupUtils::DisplayJoinedMembersL( const TDesC& aGroupId )
    {
    CDesCArray* joinedMembers = new( ELeave )CDesCArrayFlat(
        KArrayGranularity );
    CleanupStack::PushL( joinedMembers );

    iGroupUtilsPC->GetParticipantsL( *joinedMembers, aGroupId );

    joinedMembers->Sort( ECmpCollated );

    IMDialogUtils::DisplayPopupListL( joinedMembers,
                                      R_CHATVIEW_SHOW_PART_PROMPT, ELbmDoesNotOwnItemArray );

    CleanupStack::PopAndDestroy( joinedMembers );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtils::IsAdmin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtils::IsAdmin( const TDesC& aGroupId ) const
    {
    return iGroupUtilsPC->IsAdmin( aGroupId );
    }
TPtrC CCAGroupUtils::GetJoinGroupID()
    {
    if ( iJoinGroupId )
        {
        return iJoinGroupId->Des(); ;    
        }
    else
        {
        return KEmptyDesC();       
        }    
    }
