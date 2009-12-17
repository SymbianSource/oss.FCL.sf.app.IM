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

#include    "PublicEngineDefinitions.h"
#include    "CCAEngine.h"
#include    "CCAGroupUtilsPC.h"
#include    "ChatDebugPrint.h"
#include    "MCAGroupManagerInterface.h"
#include    "MCAGroupOperations.h"
#include    "MCAStoredGroups.h"
#include    "MCAExtendedStoredGroup.h"
#include    "CCAStorageManagerFactory.h"
#include    "CCAGroupPropertiesDialog.h"
#include    "ImpsCSPAllErrors.h"
#include    "MCASettings.h"
#include    "StringLoader.h"
#include    "CCAChatInfoArray.h"
#include    "CAArrayUtils.h"
#include    "IMUtils.h"
#include    "MCAInvite.h"
#include    "CAUtils.h"
#include    "MCAStoredContacts.h"
#include    "CCAStorageManagerFactory.h"
#include    "MCAConversationMessage.h"
#include	"MCAProcessManager.h"
#include    "CCAProcessManagerFactory.h"
#include    "CCAGroupPropertiesPC.h"
#include    "MCAChatInterface.h"


#include    <chatNG.rsg>
#include    <chatNG.mbg>
#include    <impsgroupprops.h>
#include    <aknnotewrappers.h>
#include    <aknlists.h>
#include    <bautils.h>
#include    "CCAPCUtils.h"
#include    "CCAUINGUtils.h"
#include    "ccagrouputilspc.h"
#include    <e32cmn.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
#include 	<CAVariationNG.rsg>

const TInt KGranularity = 2;

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::CCAGroupUtilsPC
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAGroupUtilsPC::CCAGroupUtilsPC( CCAEngine& aEngine )
        : iEngine( aEngine )


    {

    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAGroupUtilsPC* CCAGroupUtilsPC::NewLC( CCAEngine& aEngine )
    {
    CCAGroupUtilsPC* temp = new( ELeave )CCAGroupUtilsPC( aEngine );
    CleanupStack::PushL( temp );
    temp->ConstructL();
    return temp;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAGroupUtilsPC* CCAGroupUtilsPC::NewL( CCAEngine& aEngine )
    {
    CCAGroupUtilsPC* self = CCAGroupUtilsPC::NewLC( aEngine );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC: ConstructL
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::ConstructL(  )
    {
    iStoredGroups = CCAStorageManagerFactory::GroupListInterfaceL();
    iGroupManagerInterface = iEngine.GetGroupInterface();
    }
// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::~CCAGroupUtilsPC
// -----------------------------------------------------------------------------
//
CCAGroupUtilsPC::~ CCAGroupUtilsPC(  )
    {

    }
// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetChatInfoArrayLC
// -----------------------------------------------------------------------------
//
CCAChatInfoArray* CCAGroupUtilsPC::GetChatInfoArrayL( const TDesC& aGroupId )
    {

    // create listbox item array
    CCAChatInfoArray* itemArray = CCAChatInfoArray::NewL();
    CleanupStack::PushL( itemArray );

    //get the group
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );
    if ( !group )
        {
        CleanupStack::PopAndDestroy(); //itemArray
        return NULL;// leave from here
        }


    CDesCArray* userList = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( userList );
    CDesCArray* screenNames  = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( screenNames );
    CDesCArray* moders = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( moders );
    CDesCArray* admins = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( admins );

    //Getting group properties and members
    MCAGroupOperations* grOps =
        iGroupManagerInterface->GroupOperationsL( aGroupId );
    CImpsCommonGroupProps* groupProps = NULL;
    CImpsPrivateGroupProps* privateProps = NULL;

    TInt err( grOps->GetPropertiesL( groupProps, privateProps ) );
    if ( err == KErrNone && group->IsAdmin() )
        {
        err = grOps->GetMembersL( *userList, *screenNames, *moders, *admins );
        }

    if ( err != KErrNone )
        {
        if ( err == ECSPInsufficientGroupPrivileges )
            {
            // Show general server error note, if user has
            // insufficient privileges.
            err = ECSPInternalOrNetworkError;
            }
        CleanupStack::PopAndDestroy( 4, userList );
        CleanupStack::PopAndDestroy(); //itemArray

        User::Leave( err );

        return NULL;

        }

    CHAT_DP( D_CHAT_LIT( " %d users, %d screenNames, %d moders, %d admins" ),
             userList->Count(), screenNames->Count(), moders->Count(),
             admins->Count() );



    // load participants text with correct numbers (two items in partiarray)
    CArrayFixFlat<TInt>* partiArray = new ( ELeave ) CArrayFixFlat<TInt>( KGranularity );
    CleanupStack::PushL( partiArray );
    partiArray->AppendL( group->NumParticipants()  );
    partiArray->AppendL( groupProps->MaxNbrOfUsers() );
    HBufC* partiBuf = StringLoader::LoadLC( R_CHATLIST_VIEW_INFO_NUMB_PARTI,
                                            *partiArray );

    // add items to item array
    TPtrC displayId( CCAPCUtils::DisplayId( aGroupId ) );
    itemArray->AddLabelAndTextL( R_CHATLIST_VIEW_INFO_GID, displayId );

    itemArray->AddLabelAndTextL( R_CHATLIST_VIEW_INFO_TOP,
                                 groupProps->Topic() );

    itemArray->AddLabelAndTextL( R_CHATLIST_VIEW_INFO_NUMB, *partiBuf );
    CleanupStack::PopAndDestroy( partiBuf );
    CleanupStack::PopAndDestroy( partiArray );

    // add admins
    HBufC* adminBuf = CCAPCUtils::CreateBufLC( *admins, ETrue );
    itemArray->AddLabelAndTextL( R_CHATLIST_VIEW_INFO_ADMIN, *adminBuf );
    CleanupStack::PopAndDestroy( adminBuf );

    // add banned users
    if ( group->IsAdmin() || group->IsOwnGroup() )
        {
        CHAT_DP_TXT( "displaying reject list" );
        CDesCArray* rejectList = new ( ELeave ) CDesCArrayFlat(
            KArrayGranularity );
        CleanupStack::PushL( rejectList );

        err = grOps->GetRejectListL( *rejectList );
        if ( err == KErrNone )
            {
            HBufC* banBuf = CCAPCUtils::CreateBufLC( *rejectList, ETrue );
            itemArray->AddLabelAndTextL( R_CHATLIST_VIEW_INFO_BANNED, *banBuf );
            CleanupStack::PopAndDestroy( banBuf );
            }

        CleanupStack::PopAndDestroy( rejectList );
        }

    // load the whisper text
    HBufC* whispBuf = NULL;
    if ( groupProps->IsPrivateAllowed() == EImpsPropYes )
        {
        whispBuf = CCoeEnv::Static()->
                   AllocReadResourceLC( R_CHATLIST_VIEW_INFO_WHIS_YES );
        }
    else
        {
        whispBuf = CCoeEnv::Static()->
                   AllocReadResourceLC( R_CHATLIST_VIEW_INFO_WHIS_NO );
        }

    // append to listbox
    itemArray->AddLabelAndTextL( R_CHATLIST_VIEW_INFO_WHIS, *whispBuf );

    CleanupStack::PopAndDestroy( 5, userList );	// admins, moders,
    // screenNames, userList,whispBuf

    CleanupStack::Pop( itemArray );
    return itemArray;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetCreatedChatRoomListL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::GetCreatedChatRoomListL( CDesCArray& aTempArray, CDesCArray& aGroupIds )
    {
    TInt err = KErrNone;

    iGroupManagerInterface->PopulateCreatedGroupsL( aGroupIds );


    HBufC* itemText = HBufC::NewLC( KMaxChatGroupLenght + KMaxTabLength );
    TPtr itemTextPtr( itemText->Des() );
    TInt count( aGroupIds.MdcaCount() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        itemTextPtr.Append( KIconTabulator );
        itemTextPtr.Append(
            CCAStorageManagerFactory::ContactListInterfaceL()->Identification(
                aGroupIds.MdcaPoint( i ) ) );
        aTempArray.AppendL( *itemText );
        itemTextPtr.Zero();
        }
    CleanupStack::PopAndDestroy( itemText );
    return err;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::DeleteChatGroupFromNetworkL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::DeleteChatGroupFromNetworkL( const TDesC& aGroupId, TBool aDeleteFromNetwork )
    {
    MCAStoredGroups* groups =
        CCAStorageManagerFactory::GroupListInterfaceL();
    MCAStoredGroup* group = groups->FindGroup( aGroupId );
    if ( !group )
        {
        // Because the groups that user selected are fetched from
        // network, and though they might not be in our local storage,
        // we need to create those groups to make sure that storage
        // does also network operations.
        group = groups->CreateGroupL();
        static_cast<MCAExtendedStoredGroup*>( group )->SetVisible(
            EFalse );
        static_cast<MCAExtendedStoredGroup*>( group )->SetGroupIdL(
            aGroupId );
        }
    return iGroupManagerInterface->DeleteGroupL( aGroupId, aDeleteFromNetwork );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::DeleteGroupL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::DeleteGroupL( const TDesC& aGroupId,
                                    TBool  aDeleteFromNetwork )
    {
    return iGroupManagerInterface->DeleteGroupL( aGroupId,
                                                 aDeleteFromNetwork );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::SwitchToJoinedGroupL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::SwitchToJoinedGroupL( const TDesC& aGroupID,
                                            TCADnlChatViewBuf& aViewDnl ,
                                            const MCAConversationMessage* aFwdMessage,
                                            TBool aInviteQuery )

    {

    if ( !iEngine.ChatInterface().ChatExistsL( KNullDesC, KNullDesC, aGroupID ) )
        {
        return R_CHATLIST_VIEW_JOIN_ERROR;
        }

    TCADnlChatView viewDnl;
    if ( aFwdMessage )
        {
        viewDnl.iIsForwarded = ETrue;
        viewDnl.iForwardedMessage = aFwdMessage;
        }
    else
        {
        viewDnl.iIsForwarded = EFalse;
        }

    viewDnl.iInviteQuery = aInviteQuery;
    viewDnl.iSwitchTab = EFalse;

    MCAStoredGroup* group = CCAStorageManagerFactory::
                            GroupListInterfaceL()->FindGroup( aGroupID );
    viewDnl.iGroupId = aGroupID.Left( viewDnl.iGroupId.MaxLength() );
    viewDnl.iGroupName =
        group->GroupName().Left( viewDnl.iGroupName.MaxLength() );

    aViewDnl = viewDnl;

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetDefaultGroupNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------

void CCAGroupUtilsPC::GetDefaultGroupNameL( TPtr& aChatGroupName,
                                            TEnumsPC::TListSelectionType aSelType )
    {
    // Construct default group name for query

    switch ( aSelType )
        {
        case TEnumsPC::ESingleListMultiSelect:
            {
            HBufC* listGroupName = StringLoader::LoadLC(
                                       RSC_CHAT_GROUP_DEFAULT_LIST,
                                       aChatGroupName
                                   );
            aChatGroupName.Copy( listGroupName->Left( KMaxChatGroupLenght ) );

            CleanupStack::PopAndDestroy( listGroupName );
            break;
            }
        default:
            {
            TInt nbrOfChats = iGroupManagerInterface->
                              NumberOfCreatedGroupsDuringSession();
            MCAStoredContacts* contactlist =
                CCAStorageManagerFactory::ContactListInterfaceL();

            TPtrC ownId = contactlist->OwnStatus().Identification();
            HBufC* groupName = StringLoader::LoadLC( RSC_CHAT_GROUP_DEFAULT,
                                                     ownId,
                                                     nbrOfChats + 1 );
            aChatGroupName.Copy( groupName->Left( KMaxChatGroupLenght ) );

            // convert numbers to local language.
            // Exclude indentification part of text.
            TInt pos = aChatGroupName.Find( ownId );
            if ( pos != KErrNotFound )
                {
                TInt end = pos + ownId.Length();

                // after id part
                if ( end < aChatGroupName.Length() )
                    {
                    TPtr nonIdPart = aChatGroupName.MidTPtr( end );
                    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                        nonIdPart );
                    }

                // before id part
                if ( pos > 0 )
                    {
                    TPtr nonIdPart = aChatGroupName.LeftTPtr( pos );
                    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                        nonIdPart );
                    }
                }

            CleanupStack::PopAndDestroy( groupName );
            break;
            }

        }

    }


// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::ScreenNameQueryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------

void CCAGroupUtilsPC::ScreenNameQueryL( TDes& aScreenName )
    {
    TInt askScreenName = IMUtils::IntResourceValueL(
                             RSC_CHAT_VARIATION_SCREENNAME_QUERY );
    TInt screenNameStrategy = IMUtils::IntResourceValueL(
                                  RSC_CHAT_VARIATION_SCREENNAME_DEFAULT );
    TInt snSettings = IMUtils::IntResourceValueL(
                          RSC_CHAT_VARIATION_SCREENNAME_SETTING );
    TBool isSnSet = iEngine.GetInternalSettingsInterface()->Value(
                        MCASettings::EDefaultScreenNameInUse );
    TInt aliasEnabled = IMUtils::IntResourceValueL(
                            RSC_CHAT_VARIATION_HAVE_ALIAS );

    TInt snMaxLen = aScreenName.MaxLength();
    TInt copyLen = snMaxLen < KScreenNameQueryMaxLength ?
                   snMaxLen : KScreenNameQueryMaxLength;

    switch ( screenNameStrategy )
        {
        case 0 :
            {
            // use empty screen name
            aScreenName.Copy( KNullDesC() );
            break;
            }

        case 1 :
            {
            // userid, overwrite screen name buffer with userid
            HBufC* ownWVIDbuf = iEngine.GetInternalSettingsInterface()->
                                ValueL( MCASettings::EOwnWVUserID );
            CleanupStack::PushL( ownWVIDbuf );
            // make sure it can fit
            TPtrC displayId( CCAPCUtils::DisplayId( *ownWVIDbuf ) );
            aScreenName.Copy( displayId.Left( copyLen ) );
            CleanupStack::PopAndDestroy( ownWVIDbuf );

            break;
            }

        case 2 :
            {
            // screen name, default behaviour
            if ( snSettings && isSnSet )
                {
                HBufC* sn = iEngine.GetInternalSettingsInterface()->ValueL(
                                MCASettings::EDefaultScreenName );
                if ( sn->Length() > 0 )
                    {
                    aScreenName.Copy( sn->Left( copyLen ) );
                    }
                delete sn;
                }

            break;
            }

        case 3 :
            {
            // alias
            if ( aliasEnabled )
                {
                HBufC* alias = iEngine.GetInternalSettingsInterface()->OwnAliasL();
                if ( alias && alias->Length() > 0 )
                    {
                    aScreenName.Copy( alias->Left( copyLen ) );
                    }
                else
                    {
                    // If alias is not defined, we need to query screenname.
                    askScreenName = ETrue;
                    }
                delete alias;
                }

            break;
            }

        default :
            {
            break;
            }
        }
    }

// pcside_complete
// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::MoveOwnIdToFirstL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::MoveOwnIdToFirstL( CDesCArray& aList )
    {
    // find the own wvid from list and move it to the beginning
    HBufC* ownWVIDbuf = iEngine.GetInternalSettingsInterface()->
                        ValueL( MCASettings::EOwnWVUserID );
    CleanupStack::PushL( ownWVIDbuf );
    TInt listLen = aList.Count();
    // must traverse list backwards
    for ( TInt i = listLen - 1; i >= 0; i-- )
        {
        if ( 0 == CAUtils::NeutralCompare( aList.MdcaPoint( i ), *ownWVIDbuf ) )
            {
            // found own id, delete it
            aList.Delete( i );
            }
        }
    aList.InsertL( 0, *ownWVIDbuf );
    CleanupStack::PopAndDestroy( ownWVIDbuf );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::UpdateGroupMembersL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::UpdateGroupMembersL( const TDesC& aGroupId,
                                           const CDesCArray& aOldAdmins, const CDesCArray& aAdmins,
                                           const CDesCArray& aOldUserList, const CDesCArray& aUserList,
                                           const CDesCArray& aOldRejected, const CDesCArray& aRejected )
    {
    MCAGroupOperations* grOps =
        iGroupManagerInterface->GroupOperationsL( aGroupId );

    if ( !grOps )
        {
        return;
        }
    // added admins
    CHAT_DP_TXT( "**addedAdmins**" );
    CDesCArray* addedAdmins = CAArrayUtils::CreateDiffLC( aAdmins, aOldAdmins );
    // removed admins
    CHAT_DP_TXT( "**removedAdmins**" );
    CDesCArray* removedAdmins = CAArrayUtils::CreateDiffLC( aOldAdmins,
                                                            aAdmins );

    // -- removed users --
    CHAT_DP_TXT( "**removedUsers**" );
    CDesCArray* removed = CAArrayUtils::CreateDiffLC( aOldUserList, aUserList );
    // removed admins
    CAArrayUtils::AppendArrayL( *removedAdmins, *removed );
    TInt err( KErrNone );
    if ( removed->Count() > 0 )
        {
        CHAT_DP_FUNC_DP( "EditChatGroupPropertiesL", "..removed users.." );
        err = grOps->RemoveMembersL( *removed );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( 3, addedAdmins );
            User::Leave( err );// leaves on errors
            }
        }
    CleanupStack::PopAndDestroy( removed );

    // -- added users --
    CHAT_DP_TXT( "**addedUsers**" );
    CDesCArray* added = CAArrayUtils::CreateDiffLC( aUserList, aOldUserList );
    CDesCArray* empty = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( empty );
    // admins must be added first as ordinary users
    CDesCArray* addedUsers = CAArrayUtils::CloneArrayLC( *added );
    CAArrayUtils::AppendArrayL( *addedAdmins, *addedUsers );
    if ( addedUsers->Count() > 0 )
        {
        CHAT_DP_FUNC_DP( "EditChatGroupPropertiesL", "..added users.." );
        err = grOps->AddMembersL( *addedUsers, *empty );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( 5, addedAdmins );
            User::Leave( err );// leaves on errors
            }
        }
    CleanupStack::PopAndDestroy( addedUsers );
    CleanupStack::PopAndDestroy( empty );

    // -- modify the rights for new admins --
    if ( addedAdmins->Count() > 0 )
        {
        CHAT_DP_FUNC_DP( "EditChatGroupPropertiesL", "..added admins.." );
        err = grOps->ModifyMembersL( addedAdmins, NULL, NULL );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( 3, addedAdmins );
            User::Leave( err );// leaves on errors
            }

        }

    CleanupStack::PopAndDestroy( 3, addedAdmins );

    // -- added or removed banned users --
    CHAT_DP_TXT( "**addedRejected**" );
    CDesCArray* addedBan = CAArrayUtils::CreateDiffLC( aRejected,
                                                       aOldRejected );
    CHAT_DP_TXT( "**removedRejected**" );
    CDesCArray* removedBan = CAArrayUtils::CreateDiffLC( aOldRejected,
                                                         aRejected );
    if ( ( addedBan->Count() > 0 ) || ( removedBan->Count() > 0 ) )
        {
        CHAT_DP_FUNC_DP( "EditChatGroupPropertiesL", "..added/removed ban.." );
        err = grOps->SetRejectListL( *addedBan, *removedBan );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( 2, addedBan );
            User::Leave( err );// leaves on errors
            }
        }

    CleanupStack::PopAndDestroy( 2, addedBan );

    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::AddGroupMembersL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::AddGroupMembersL( const TDesC& aGroupId,
                                        CDesCArray& aAdmins,
                                        const CDesCArray& aUserList,
                                        const CDesCArray& aRejected )
    {
    MCAGroupOperations* grOps = iGroupManagerInterface->
                                GroupOperationsL( aGroupId );

    if ( !grOps )
        {
        return;
        }
    if ( aAdmins.Count() > 0 )
        {
        // remove own wvid from list
        aAdmins.Delete( 0 );
        aAdmins.Compress();
        }

    TInt err( KErrNone );
    CDesCArray* empty = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( empty );
    CDesCArray* added = CAArrayUtils::CloneArrayLC( aUserList );
    CAArrayUtils::AppendArrayL( aAdmins, *added );
    if ( added->MdcaCount() > 0 )   // added users
        {
        err = grOps->AddMembersL( *added, *empty );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( 2, empty );
            User::Leave( err );// leaves on errors
            }
        }
    CleanupStack::PopAndDestroy( added );

    // -- modify the rights for new admins (ignore own wvid) --
    if ( aAdmins.Count() > 0 )
        {
        err = grOps->ModifyMembersL( &aAdmins, NULL, NULL );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy(); //empty
            User::Leave( err );// leaves on errors
            }

        }
    // -- added banned users --
    if ( aRejected.Count() > 0 )
        {
        err = grOps->SetRejectListL( aRejected, *empty );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy(); //empty
            User::Leave( err );// leaves on errors
            }

        }
    CleanupStack::PopAndDestroy(); //empty
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetRejectListL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::GetRejectListL( CDesCArray& aRejectList,
                                      const TDesC& aGroup )
    {
    // populate lists and get the properties
    MCAGroupOperations* grOps = iGroupManagerInterface->GroupOperationsL( aGroup );
    return grOps->GetRejectListL( aRejectList );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetMembersL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::GetMembersL( CDesCArray& aUserList,
                                   CDesCArray& aScreenNames,
                                   CDesCArray& aModers,
                                   CDesCArray& aAdmins , const TDesC& aGroup )
    {
    MCAGroupOperations* grOps = iGroupManagerInterface->GroupOperationsL( aGroup );
    return grOps->GetMembersL( aUserList,
                               aScreenNames,
                               aModers,
                               aAdmins );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetIdentificationL
// -----------------------------------------------------------------------------
//
const TPtrC CCAGroupUtilsPC::GetIdentificationL( const TDesC& aContactId )
    {
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    return contacts->Identification( aContactId ) ;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::CreateGroupSimpleL
// -----------------------------------------------------------------------------
//
HBufC* CCAGroupUtilsPC::CreateGroupSimpleL( const TDesC& aScreenName,
                                            TBool aJoinGroup,
                                            TBool aIsWhisperingEnabled, TInt& aErrorCode )
    {
    CImpsPrivateGroupProps* newPrivateProps = CImpsPrivateGroupProps::NewL();
    CleanupStack::PushL( newPrivateProps );

    MCAProcessManager* processManager
    = CCAProcessManagerFactory::InstanceL()->GetProcessManager();

    CCAGroupPropertiesPC* groupPropsDialogPC
    = dynamic_cast<CCAGroupPropertiesPC*>( processManager->GetGroupPropertiesPC() );

    HBufC* groupID =  iGroupManagerInterface->CreateGroupL(
                          groupPropsDialogPC->GetEdittedProperties(),
                          newPrivateProps,
                          aScreenName,
                          aJoinGroup,
                          aIsWhisperingEnabled,
                          aErrorCode );

    CleanupStack::PopAndDestroy( newPrivateProps );
    return groupID;
    }


// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::CreateGroupL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAGroupUtilsPC::CreateGroupL( const TDesC& aScreenName, const TDesC& aGroupName,
                                      TInt& aError )
    {
    // finalize chat group creation and join it, then send all the
    // invitations
    // create chat group with default properties
    CImpsCommonGroupProps* groupProps =
        CImpsCommonGroupProps::NewL();
    CleanupStack::PushL( groupProps );
    CImpsPrivateGroupProps* privateProps =
        CImpsPrivateGroupProps::NewL();
    CleanupStack::PushL( privateProps );

    // set properties:
    // no search, open group, no whisper, empty topic and welcome
    groupProps->SetMaxNbrOfUsers(
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_SIZE ) );
    groupProps->SetSearchable(
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_SEARCH ) == 1 ?
        EImpsPropYes : EImpsPropNo );
    groupProps->SetOpen(
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_OPEN ) == 1 ?
        EImpsPropYes : EImpsPropNo );
    groupProps->SetPrivateAllowed(
        IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_WHISPER ) == 1 ?
        EImpsPropYes : EImpsPropNo );
    groupProps->SetGroupNameL( aGroupName );

    HBufC* topic = StringLoader::LoadLC( RSC_CRRSS_CHAT_VARIATION_START_ID +
                                         RSC_CHAT_VARIATION_GROUP_TOPIC );
    HBufC* welcome = StringLoader::LoadLC( RSC_CRRSS_CHAT_VARIATION_START_ID +
                                           RSC_CHAT_VARIATION_GROUP_WELCOME_NOTE );
    groupProps->SetTopicL( *topic );
    groupProps->SetWelcomeL( *welcome );
    CleanupStack::PopAndDestroy( 2, topic ); // welcome, topic


    // check the autodeletion variation flag
    if ( IMUtils::IntResourceValueL( RSC_CHAT_VARIATION_GROUP_AUTODELETE ) )
        {
        groupProps->SetAutoDelete( EImpsPropYes );
        }

    CleanupStack::Pop( 2, groupProps ); // privateProps, groupProps

    // CreateGroupL takes ownership
    TBool whisperingAllowed =
        IMUtils::IntResourceValueL(
            RSC_CHAT_VARIATION_GROUP_WHISPER ) == 1;
    return iGroupManagerInterface->CreateGroupL( groupProps,
                                                 privateProps,
                                                 aScreenName,
                                                 EFalse,
                                                 whisperingAllowed,
                                                 aError ) ;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::JoinGroupL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::JoinGroupL( const TDesC& aGroupId,
                                  const TDesC& aScreenName,
                                  TBool aUsers, TBool aIsFavourite )

    {

    TBool whisperingAllowed = IMUtils::IntResourceValueL(
                                  RSC_CHAT_VARIATION_GROUP_WHISPER ) == 1;
    return iGroupManagerInterface->JoinGroupL(
               aGroupId, aScreenName, aUsers,
               aIsFavourite, whisperingAllowed );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetGroupDataL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::GetGroupDataL( const TDesC& aGroupId,
                                     CDesCArray& aUserList, CDesCArray& aScreenNames,
                                     CDesCArray& aModerators, CDesCArray& aAdmins,
                                     CDesCArray& aRejected )
    {

    // populate lists and get the properties
    MCAGroupOperations* grOps =
        iGroupManagerInterface->GroupOperationsL( aGroupId );



    CImpsCommonGroupProps* groupProps = NULL;
    CImpsPrivateGroupProps* privateProps = NULL;

    TInt errMem = grOps->GetMembersL( aUserList, aScreenNames, aModerators, aAdmins );
    TInt errRej = grOps->GetRejectListL( aRejected );
    TInt errProp = grOps->GetPropertiesL( groupProps, privateProps );

    CHAT_DP( D_CHAT_LIT(
                 "CCAGroupUtilsPC::GetGroupDataL: GetMembers(%d), GetRejectList(%d), GetProperties(%d)" ),
             errMem, errRej, errProp );

    if ( errProp != KErrNone )
        {
        return ( errProp == ECSPInsufficientGroupPrivileges ) ?
               ECSPInternalOrNetworkError : errProp;
        }

    // Ignore other errors
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::ProcessEditedGroupPropertiesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::ProcessEditedGroupPropertiesL( const TDesC& aGroupId,
                                                     CDesCArray& aOldAdmins,
                                                     CDesCArray& aNewAdmins,
                                                     CDesCArray& aOldUsers,
                                                     CDesCArray& aNewUsers,
                                                     CDesCArray& aOldRejected,
                                                     CDesCArray& aNewRejected )
    {

    // In the case of an open group, we must 1. update properties, 2. update members
    // In the case of a closed group, we must 1. update members, 2. update properties
    // Otherwise we end up kicking people out in one of the cases
    // populate lists and get the properties
    MCAGroupOperations* grOps =
        iGroupManagerInterface->GroupOperationsL( aGroupId );
    MCAProcessManager* processManager
    = CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAGroupPropertiesPC* groupPropsDialogPC = dynamic_cast<CCAGroupPropertiesPC*>
                                               ( processManager->GetGroupPropertiesPC() );
    TInt err( KErrNone );
    TBool isOpen = ( groupPropsDialogPC->
                     GetEdittedProperties()->IsOpen() == TEnumsPC::EImpsPropYes );

    CImpsPrivateGroupProps* newPrivateProps = CImpsPrivateGroupProps::NewL();
    CleanupStack::PushL( newPrivateProps );


    if ( isOpen )
        {
        // open group

        // Update properties
        err = grOps->UpdatePropertiesL(
                  groupPropsDialogPC->GetEdittedProperties(),
                  newPrivateProps );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy(); //newPrivateProps
            return err;
            }

        // Update group members
        UpdateGroupMembersL( aGroupId, aOldAdmins, aNewAdmins,
                             aOldUsers, aNewUsers, aOldRejected, aNewRejected );
        }
    else
        {
        // closed group

        // Update group members
        UpdateGroupMembersL( aGroupId, aOldAdmins, aNewAdmins,
                             aOldUsers, aNewUsers, aOldRejected, aNewRejected );

        // Update properties
        err = grOps->UpdatePropertiesL( groupPropsDialogPC->GetEdittedProperties(),
                                        newPrivateProps );
        if ( err != KErrNone )
            {
            CleanupStack::PopAndDestroy(); //newPrivateProps
            return err;
            }
        }

    // update the group name
    MCAExtendedStoredGroup* group = static_cast<MCAExtendedStoredGroup*>
                                    ( iStoredGroups->FindGroup( aGroupId ) );

    if ( group )
        {
        group->SetGroupNameL( groupPropsDialogPC->
                              GetEdittedProperties()->GroupName() );
        if ( group->StorageType() == TStorageManagerGlobals::EStoragePersistent )
            {
            group->SaveChangesL();
            }
        // signal the ui of changed group
        iStoredGroups->SignalGroupChangedL( aGroupId );
        }

    CleanupStack::PopAndDestroy( 1, newPrivateProps );

    return KErrNone;
    }
// ---------------------------------------------------------
// CCAGroupUtilsPC::SaveAsFavouriteL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupUtilsPC::SaveAsFavouriteL( const TDesC& aGroupId )
    {

    MCAStoredGroup* chatGroup = iStoredGroups->FindGroup( aGroupId );
    if ( chatGroup )
        {
        MCAExtendedStoredGroup* exGrp = static_cast <
                                        MCAExtendedStoredGroup* > ( chatGroup );
        if ( exGrp )
            {
            exGrp->SaveChangesL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::CreateOrJoinGroupL
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::CreateOrJoinGroupL( const TDesC& aGroupId,
                                          TBool& aIsJoined, TBool& aIsFavourite )
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    if ( group )
        {
        aIsJoined = group->IsJoined();

        aIsFavourite = ( group->StorageType() ==
                         TStorageManagerGlobals::EStoragePersistent );
        }
    else
        {
        // if the group doesn't exist, create it
        CHAT_DP_FUNC_DP( "JoinGroupL", "created new group" );
        // Ownership not transferred to caller
        MCAExtendedStoredGroup* extGroup = iStoredGroups->CreateGroupL();
        // CSI: 35 # Comment above

        extGroup->SetGroupIdL( aGroupId );
        extGroup->SetGroupNameL( KNullDesC );

        group = extGroup;
        }
    currentGroup = group;

    // set the group visible, just in case
    static_cast<MCAExtendedStoredGroup*>( group )->SetVisible( ETrue );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetEngineDefaultGroupName
// -----------------------------------------------------------------------------
//
TPtrC CCAGroupUtilsPC::GetEngineDefaultGroupName( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );
    if ( group )
        {
        return group->GroupName();
        }
    else
        {

        CImpsCommonGroupProps* commonProps = NULL;

        // if the group doesn't exist, create a temp one
        CHAT_DP_FUNC_DP( "JoinGroupL", "created new group" );

        TRAPD( error,
               // Ownership not transferred to caller
               MCAExtendedStoredGroup* extGroup = iStoredGroups->CreateGroupL();

               extGroup->SetGroupIdL( aGroupId );
               extGroup->SetGroupNameL( KNullDesC );

               // Set the group invisible
               static_cast<MCAExtendedStoredGroup*>( extGroup )->SetVisible( EFalse );

               MCAGroupOperations* grOps = iGroupManagerInterface->GroupOperationsL( aGroupId );

               // if we created the group, fetch the name for it
               CImpsPrivateGroupProps* privProps = NULL;
               TInt err = grOps->GetPropertiesL( commonProps, privProps );
             );

        if ( error == KErrNone && commonProps != NULL )
            {
            return commonProps->GroupName();
            }
        // If we can not get the group name return KNullDesC
        else
            {
            return KNullDesC();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::SetGroupNameL
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::SetGroupNameL( const TDesC& aGroupId )
    {
    MCAGroupOperations* grOps = iGroupManagerInterface->GroupOperationsL( aGroupId );
    // if we created the group, fetch the name for it
    CImpsCommonGroupProps* commonProps = NULL;
    CImpsPrivateGroupProps* privProps = NULL;
    TInt err = grOps->GetPropertiesL( commonProps, privProps );

    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    if ( err == KErrNone && commonProps )
        {
        static_cast<MCAExtendedStoredGroup*>( currentGroup )
        ->SetGroupNameL( commonProps->GroupName() );
        }

    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetAliasL
// -----------------------------------------------------------------------------
//
HBufC* CCAGroupUtilsPC::GetAliasL()
    {
    return iEngine.GetInternalSettingsInterface()->ValueL( MCASettings::EOwnAlias );
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::GetGroupNameForJoinedGroupL
// -----------------------------------------------------------------------------
//
TPtrC CCAGroupUtilsPC::GetGroupNameForJoinedGroupL( const TDesC& aGroupId )
    {
    // update the group name

    MCAGroupOperations* grOps = iGroupManagerInterface->GroupOperationsL( aGroupId );

    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    // prefer showing the group name
    if ( group->GroupName().Length() == 0 )
        {
        // no group name, use Group ID
        return CCAPCUtils::DisplayId( aGroupId );
        }
    else
        {
        // group name exists
        return group->GroupName();
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::DeleteFailedGroupL
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::DeleteFailedGroupL( const TDesC& aGroupId,
                                          TBool aDeleteFromNetwork )
    {

    // update the group name
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    if ( ( !group->IsOwnGroup() ) &&
         ( group->StorageType() !=
           TStorageManagerGlobals::EStoragePersistent ) &&
         ( !group->IsJoined() ) )
        {

        iStoredGroups->SetGroupDeleted( ETrue );
        // if join was not succesfull and group is not own nor saved,
        // remove the group from manager
        iGroupManagerInterface->DeleteGroupL( aGroupId, aDeleteFromNetwork );

        iStoredGroups->SetGroupDeleted( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::SendInvitationsL
// -----------------------------------------------------------------------------
//
TInt CCAGroupUtilsPC::SendInvitationsL( const CDesCArray& aUserIDs,
                                        const TDesC& aGroupID,
                                        const TDesC& aMessage )
    {
    TInt retVal = KErrNone;
    // we have invitations, so send them
    CCAInvitationRequest* invitation =
        CCAInvitationRequest::NewL( aUserIDs,
                                    aGroupID,
                                    aMessage );

    CleanupStack::PushL( invitation );


    MCAInvite* inviteAPI = iEngine.GetInvitationInterface();
    // Send invitation
    inviteAPI->SendInvitationL( *invitation );
    CleanupStack::PopAndDestroy( invitation );
    return retVal;
    }


// -----------------------------------------------------------------------------
// CCAGroupUtilsPC: FindGroup
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtilsPC::FindGroup( const TDesC& aId )
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aId );
    if ( group )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCAGroupUtilsPC: GetParticipantsL
// -----------------------------------------------------------------------------
//
void CCAGroupUtilsPC::GetParticipantsL( CDesCArray& aParticipantList,
                                        const TDesC& aGroupID ) const
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupID );
    if ( group )
        {
        group->GetParticipantsL( aParticipantList );
        }
    }


// ---------------------------------------------------------
// CCAGroupUtilsPC::LeaveGroupL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupUtilsPC::LeaveGroupL( const TDesC& aGroupId )
    {
    return  iGroupManagerInterface->LeaveGroupL( aGroupId ) ;
    }


// -----------------------------------------------------------------------------
// CCAGroupUtilsPC::IsFavouriteChatGroup
// -----------------------------------------------------------------------------
//
TBool CCAGroupUtilsPC::IsFavouriteChatGroup( const TDesC& aGroupId ) const
    {
    TBool isFavourite( EFalse );
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );
    if ( group )
        {
        isFavourite = ( group->StorageType() ==
                        TStorageManagerGlobals::EStoragePersistent );
        }
    return isFavourite;
    }

// ---------------------------------------------------------
// CCAGroupUtilsPC::IsOwnGroup
// ---------------------------------------------------------
//
TBool CCAGroupUtilsPC::IsOwnGroup( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    if ( group )
        {
        return group->IsOwnGroup();
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CCAGroupUtilsPC::IsJoined
// ---------------------------------------------------------
//
TBool CCAGroupUtilsPC::IsJoined( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );

    if ( group )
        {
        return group->IsJoined();
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CCAGroupUtilsPC::IsAdmin
// ---------------------------------------------------------
//
TBool CCAGroupUtilsPC::IsAdmin( const TDesC& aGroupId ) const
    {
    MCAStoredGroup* group = iStoredGroups->FindGroup( aGroupId );
    if ( group )
        {
        return group->IsAdmin();
        }
    else
        {
        return EFalse;
        }
    }

//    End of File
