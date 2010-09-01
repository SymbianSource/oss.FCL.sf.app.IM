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
* Description:  General utility class
*
*/



// INCLUDE FILES
#include    "IMUtils.h"
#include    "CAExternalInterface.h"
#include    "chatdefinitions.h"
#include    "ChatDebugPrint.h"
#include    "CAUtils.h"
#include    "IMPrivateCRKeys.h"

#include    "CCAWaitNote.h"
#include    "IMDialogUtils.h"
#include    <ChatNG.rsg>
#include    <hlplch.h>
#include    <csxhelp/imng.hlp.hrh>
#include    <eikmenup.h>

#include    <PEngWVPresenceErrors2.h>   // wv error codes
#include    <IMPSServiceSettingsUINGInternalCRKeys.h>
#include    <CentralRepository.h>
#include    <barsread.h>
#include    <gulutil.h>
#include    <gulicon.h>
#include	<StringLoader.h>
#include    <sysutil.h>
#include    <caf/content.h>
#include    <caf/manager.h>
#include    <oma2agent.h>
#include    <collate.h>
#include	"PublicEngineDefinitions.h"

////401-1815 Ease of Instant Messaging branding
//Code added for the above requirment
#include 	"VariantKeys.h"
#include 	<CAVariationNG.rsg>



// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "IMUtilsNG" );


#define 	KLibNotInitialized		KErrBadLibraryEntryPoint
//End of code for CR 401-1815


// ---------------------------------------------------------
// IMUtils::PopulateFormattedOnlineListL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::PopulateFormattedOnlineListL( const MDesCArray& aRawOnlineList,
                                                     CDesCArray& aFormattedOnlineList )
    {
    // Add formatted data to second list
    const TInt onlineCount( aRawOnlineList.MdcaCount() );
    HBufC* formattedWVID = HBufC::NewLC( KWVUserIDMaxLength + KIconTabulator().Size() );
    TPtr formattedWVIDPtr( formattedWVID->Des() );

    for ( TInt i( 0 ); i < onlineCount; ++i )
        {
        formattedWVIDPtr.Append( KIconTabulator );
        formattedWVIDPtr.Append( aRawOnlineList.MdcaPoint( i ) );
        aFormattedOnlineList.AppendL( *formattedWVID );
        formattedWVIDPtr.Zero();
        }

    CleanupStack::PopAndDestroy( formattedWVID );
    }

// ---------------------------------------------------------
// IMUtils::LaunchHelpL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::LaunchHelpL( const TDesC& aContext )
    {
    //make context array
    //granurality 1 is ok cos there is added just one item
    CArrayFix< TCoeHelpContext >* cntx = new( ELeave ) CArrayFixFlat< TCoeHelpContext >( 1 );
    CleanupStack::PushL( cntx );

    cntx->AppendL( TCoeHelpContext( KUidChatClient, aContext ) );
    CleanupStack::Pop( cntx );

    //and launch help - takes ownership of context array
    HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(), cntx );
    }

// ---------------------------------------------------------
// IMUtils::WVSettingsChatLoginValue()
// (other items were commented in a header).
// Central Repository version
// ---------------------------------------------------------
//
EXPORT_C TInt IMUtils::WVSettingsChatLoginValue( )
    {
    CHAT_DP( D_CHAT_LIT( "CCAUISessionManager::LoginType" ) );

    TInt readValue( KErrCouldNotConnect ); // initialize to an error value

    // get the login type from central repository
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( KWVSettingsCenRepUid ) );

    if ( err )
        {
        // problems, return the error
        return err;
        }

    err = repository->Get( KIMPSCRChatLogin, readValue );

    delete repository;

    if ( err != KErrNone )
        {
        // there was an error
        return err;
        }

    return readValue;
    }

// ---------------------------------------------------------
// IMUtils::NeutralFind
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMUtils::NeutralFind( const MDesCArray& aUserList, const TDesC& aUserId )
    {
    TInt count( aUserList.MdcaCount() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( CAUtils::NeutralCompare( aUserList.MdcaPoint( i ),
                                      aUserId ) == 0 )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// IMUtils::IntResourceValueL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMUtils::IntResourceValueL( TInt aResourceId )
    {

    TInt val( 0 );
    TInt err ( KErrNone );
    ////401-1815 Ease of Instant Messaging branding
    //Code added for the above requirment

    //get the exact cenrep id for the aResourceId
    TInt key = aResourceId + KIMStartVariationID;

    err = TlsData()->RepositoryInstance().Get( key, val );

    if ( err != KErrNone )
        {
        val = IntResourceValueFromRssL( RSC_CRRSS_CHAT_VARIATION_START_ID + aResourceId );
        }
    // end of code for 401-1815 Ease of Instant Messaging Branding.
    return val;
    }



////401-1815 Ease of Instant Messaging branding
//Code added for the above requirment
// ---------------------------------------------------------
// IMUtils::IntResourceValueFromRssL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMUtils::IntResourceValueFromRssL( TInt aResourceId )
    {

    TInt val( 0 );

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    val = ResourceUtils::ReadTInt32L( reader );
    CleanupStack::PopAndDestroy(); // reader

    return val;
    }
// end of code for 401-1815 Ease of Instant Messaging Branding.


// ---------------------------------------------------------
// IMUtils::MaxMsgLength
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt IMUtils::MaxMsgLength()
    {
    TInt maxLength( 0 );
    TRAPD( err, maxLength = IntResourceValueL( RSC_CHAT_VARIATION_MESSAGE_MAX_LENGTH ) );
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    if ( maxLength <= 0 )
        {
        // if the resource value was invalid, assign default value
        maxLength = KDefaultMsgLength;
        }

    return maxLength;
    }


// ---------------------------------------------------------
// IMUtils::GetCRKeyL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUint32 IMUtils::CRKeyL( TUid aUid, TUint32 aKey )
    {
    TInt val( 0 );

    TRAPD( err,
           CRepository* rep = CRepository::NewL( aUid );
           rep->Get( aKey, val );
           delete rep;
         );

    if ( err != KErrNotFound && err != KErrPathNotFound )
        {
        // "Not found" errors are ignored, because it's ok to
        // return zero if key is not found
        User::LeaveIfError( err );
        }

    return val;
    }


// ---------------------------------------------------------
// IMUtils::AddMenuItemL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::AddMenuItemL( TBool aAdd, CEikMenuPane& aMenuPane, TInt aTextResourceId,
                                     TInt aCommandId, TInt aCascadeId /*= 0*/ )
    {
    if ( aAdd )
        {
        HBufC* text = CCoeEnv::Static()->AllocReadResourceLC( aTextResourceId );

        CEikMenuPaneItem::SData data;
        data.iCommandId = aCommandId;
        data.iCascadeId = aCascadeId;
        data.iFlags = 0;
        data.iText.Copy( text->Des().Left( CEikMenuPaneItem::SData::ENominalTextLength ) );
        data.iExtraText.Zero();
        aMenuPane.AddMenuItemL( data );

        CleanupStack::PopAndDestroy( text );
        }
    }

// ---------------------------------------------------------
// IMUtils::SetIcon
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::SetIcon( CGulIcon& aTarget, CGulIcon& aSource )
    {
    aTarget.SetBitmap( aSource.Bitmap() );
    aTarget.SetMask( aSource.Mask() );
    aTarget.SetBitmapsOwnedExternally( aSource.BitmapsOwnedExternally() );
    if ( !aSource.BitmapsOwnedExternally() )
        {
        aSource.SetBitmapsOwnedExternally( ETrue );
        }
    }

// ---------------------------------------------------------
// IMUtils::UpdateIcon
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::UpdateIcon( CGulIcon*& aTarget, CGulIcon* aSource )
    {
    if ( aTarget )
        {
        IMUtils::SetIcon( *aTarget, *aSource );
        CleanupStack::PopAndDestroy();
        }
    else
        {
        aTarget = aSource;
        CleanupStack::Pop();
        }

    }
// ---------------------------------------------------------
// IMUtils::IgnoreOfflineStatusL()
// ---------------------------------------------------------
//
EXPORT_C TInt IMUtils::IgnoreOfflineStatusL( const TDesC& aContactIdentification )
    {
    // These flags cannot be enabled simultaneously (UI spec.)
    TInt ignoreOffline = IntResourceValueL(
                             RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE );
    TInt ignoreOfflineNotify = IntResourceValueL(
                                   RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE_NOTIFY );

    // If RSC_CHAT_VARIATION_SEND_MSG_IGNORE_OFFLINE flag is set
    // offline state is ignored without any notifications
    TInt result = EAknSoftkeyOk;

    if ( !ignoreOffline )
        {
        if ( ignoreOfflineNotify )
            {
            // Show note about offline contact
            HBufC* msg = StringLoader::LoadLC(
                             R_QTN_CHAT_CONTACT_MAY_NOT_RECEIVE_MESSAGE,
                             aContactIdentification );

            result = IMDialogUtils::DisplayQueryDialogL(
                         R_CHAT_CONTACT_MAY_NOT_RECEIVE_MESSAGE_DIALOG,
                         msg->Des() );
            CleanupStack::PopAndDestroy( msg );
            }
        else
            {
            // Logged in and contact not online,
            // ignorance of offline state disabled
            IMDialogUtils::DisplayInformationNoteL(
                R_QTN_CHAT_FRIEND_OFFLINE );
            result = EVariationDoesNotAllow;
            }
        }

    if ( result == EAknSoftkeyOk )
        {
        return EIgnoredByUser;
        }
    else if ( result == 0 ) // Dialog returns 0 if cancelled by user
        {
        return EUserCanceled;
        }
    else
        {
        return result;
        }
    }

// ---------------------------------------------------------
// IMUtils::FFSSpaceBelowCriticalLevelL()
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::FFSSpaceBelowCriticalLevelL( TInt aRequestedSpace /*= 0*/
                                                    , TBool
#ifdef _DEBUG
                                                    aEmulateMemOut /*= EFalse*/
#endif // _DEBUG
                                                  )
    {
#ifdef _DEBUG
    if ( aEmulateMemOut )
        {
        User::Leave( KErrDiskFull );
        }
#endif // _DEBUG

    // Check disk space
    if ( SysUtil::FFSSpaceBelowCriticalLevelL(
             &CEikonEnv::Static()->FsSession(), aRequestedSpace ) )
        {
        User::Leave( KErrDiskFull );
        }
    }

// ---------------------------------------------------------
// IMUtils::FileProtectedL()
// ---------------------------------------------------------
//
EXPORT_C TBool IMUtils::FileProtectedL( const TFileName& aFileName )
    {
    TInt value( 0 );
    ContentAccess::CContent* content = ContentAccess::CContent::NewL( aFileName );
    TInt err( content->GetAttribute( ContentAccess::EIsProtected, value ) );
    delete content;

    return( err == KErrNone && value );
    }

// ---------------------------------------------------------
// IMUtils::ContentProtectedL()
// ---------------------------------------------------------
//
EXPORT_C TBool IMUtils::ContentProtectedL( const TDesC8& aContent )
    {
    // Note that if this method is needed often, the list
    // of agents should be cached!

    RArray<ContentAccess::TAgent> agents;
    CleanupClosePushL( agents );
    ContentAccess::TAgent agentDRM;

    // get the list of agents
    ContentAccess::CManager* manager = ContentAccess::CManager::NewLC();
    manager->ListAgentsL( agents );

    // find the DRM agent
    TInt count( agents.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( agents[i].Name().Compare( KOmaDrm2AgentName ) == 0 )
            {
            agentDRM = agents[i];
            break;
            }
        }

    HBufC8* empty = HBufC8::NewLC( 1 );
    TPtr8 output( empty->Des() );

    // execute agent specific command for OMA2 agent
    TInt ret( manager->AgentSpecificCommand( agentDRM, ContentAccess::EBufferContainsOma1Dcf,
                                             aContent, output ) );

    CleanupStack::PopAndDestroy( 3 ); // empty, manager, agents.Close()
    return ( ret != KErrNotFound );
    }

// ---------------------------------------------------------
// IMUtils::CombineStringFromResourceLC()
// See header for details.
// ---------------------------------------------------------
//
EXPORT_C HBufC* IMUtils::CombineStringFromResourceLC( TInt aResourceId1, TInt aResourceId2 )
    {
    // empty texts
    _LIT ( KPSUIEmptyStringHeader, "%S\n%S" );
    HBufC* emptyText = CCoeEnv::Static()->AllocReadResourceLC(
                           aResourceId1 );

    HBufC* emptyText2 = CCoeEnv::Static()->AllocReadResourceLC(
                            aResourceId2 );

    HBufC* emptyFinal = HBufC::NewLC(
                            emptyText->Length() + emptyText2->Length() + 2 );// 2 is length of "\n"

    CleanupStack::Pop( emptyFinal );

    emptyFinal->Des().Format( KPSUIEmptyStringHeader, emptyText, emptyText2 );

    CleanupStack::PopAndDestroy( 2 );// emptyText, emptyText2

    CleanupStack::PushL( emptyFinal );

    return emptyFinal;

    }



////401-1815 Ease of Instant Messaging branding
//Code added for the above requirment
// ---------------------------------------------------------
// IMUtils::InitializeLibL()
// See header for details.
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::InitializeLibL()
    {
    if ( !Dll::Tls() )
        {
        Dll::SetTls( CIMUtilsTlsData::NewL() );
        }

    }


// ---------------------------------------------------------
// IMUtils::UnInitializeLib()
// See header for details.
// ---------------------------------------------------------
//
EXPORT_C void IMUtils::UnInitializeLib()
    {
    if ( Dll::Tls() )
        {
        CIMUtilsTlsData* tlsData = TlsData();

        if ( TlsData() )
            {
            delete tlsData;
            Dll::SetTls( NULL );
            }
        }
    }



// -----------------------------------------------------------------------------
// IMUtils::TlsData()
// -----------------------------------------------------------------------------
//
CIMUtilsTlsData* IMUtils::TlsData()
    {
    CIMUtilsTlsData* tlsData = STATIC_CAST( CIMUtilsTlsData*, Dll::Tls() );

    __ASSERT_ALWAYS( tlsData,
                     User::Panic( KPanicCategory, KLibNotInitialized ) );

    return tlsData;
    }





// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMUtilsTlsData::CIMUtilsTlsData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMUtilsTlsData::CIMUtilsTlsData()
    {
    }

// -----------------------------------------------------------------------------
// CIMUtilsTlsData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMUtilsTlsData::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidIMNG );
    }

// -----------------------------------------------------------------------------
// CIMUtilsTlsData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMUtilsTlsData* CIMUtilsTlsData::NewL()
    {
    CIMUtilsTlsData* self = new( ELeave ) CIMUtilsTlsData;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CIMUtilsTlsData::~CIMUtilsTlsData()
    {
    delete 	iRepository;
    }


// -----------------------------------------------------------------------------
// CIMUtilsTlsData::RepositoryInstance
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CRepository& CIMUtilsTlsData::RepositoryInstance()
    {
    return *iRepository;
    }

//// End of code for 401-1815 Ease of Instant Messaging brand

//  End of File
