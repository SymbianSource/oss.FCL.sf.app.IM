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
* Description:  Process Manager to get process interfaces e.g. login,settings
*
*/



// INCLUDE FILES
#include	"CCAProcessManager.h"

#include    "MCALoggerHeadersInterface.h"
#include    "CCARecordedChatsPC.h"
#include    "CCARecordedChatsArrayPC.h"
#include    "MCAProcessManager.h"
#include	"MCAMainViewArrayPC.h"
#include	"CCALoginPC.h"

#include	"CCASettingsPC.h"

#include	"CCAConversationPC.h"

#include 	"CCAEngine.h"
#include    "CAPresenceManager.h"

#include    "MCAInvite.h"
#include    "MCAChatInterface.h"
#include    "CCAStorageManagerFactory.h"
#include    "mcamessageerrorinformer.h"
#include    "MCAStoredContact.h"
#include    "MCAStoredContacts.h"


#include 	"CCAContactEditPC.h"
#include    "CCABlockingPC.h"
#include    "CCASearchDataPC.h"
#include    "CCASearchInterfacePC.h"
#include 	"MCASearchInterface.h"
#include	"ccareactiveauthpc.h"
#include	"CCAMessageFlowHandlerPC.h"
#include 	"CCAInvitationPC.h"
#include    "MCAInvitationPC.h"
#include    "CCAGroupPC.h"
#include	"CCAGroupUtilsPC.h"
#include    "CCAInviteEditorPC.h"
#include    "MCAInviteEditorPC.h"
#include    "CCAGlobalNotificationObserverPC.h"
#include    "MCAGlobalNotificationObserverPC.h"
#include    "MCAGroupPropertiesPC.h"
#include    "CCAGroupPropertiesPC.h"
#include    "CCAApplicationNGPC.h"

#include 	"CCAOpenChatsPC.h"

// FORWARD DECLARATIONS
class MCABackgroundInterface;

//const declarations
const TUid KUidChatClient = {0x101F4673};



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCAProcessManager::NewL
// -----------------------------------------------------------------------------
//

EXPORT_C CCAProcessManager* CCAProcessManager::NewL()
    {
    CCAProcessManager* self = new ( ELeave ) CCAProcessManager();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::CreateOpenChatsPCL
// -----------------------------------------------------------------------------
//

EXPORT_C MCAOpenChatsPC* CCAProcessManager::CreateOpenChatsPCL(
    const RPointerArray<MCAArrayItemPC>& aOpenChatsArray )
    {

    MCAOpenChatsPC* openChatsPC = CCAOpenChatsPC::NewL( aOpenChatsArray );

    //Ownership is transfered to the caller.
    return openChatsPC;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetEngine
// -----------------------------------------------------------------------------
//

EXPORT_C CCAEngine* CCAProcessManager::GetEngine() const
    {
    return iEngine;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetLogInInterface
// -----------------------------------------------------------------------------
//

EXPORT_C MCALoginPC* CCAProcessManager::GetLogInInterface( ) const
    {
    return iCCALogIn;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetSettingsInterface
// -----------------------------------------------------------------------------
//

EXPORT_C MCASettingsPC* CCAProcessManager::GetSettingsInterface( ) const
    {
    return iSettingsPC;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetInvitationsInterface
// -----------------------------------------------------------------------------
//

EXPORT_C MCAInvitationPC* CCAProcessManager::GetInvitationsInterface( ) const
    {
    return iInvitationsPC;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetArrayInterface
// -----------------------------------------------------------------------------
//

EXPORT_C MCAMainViewArrayPC* CCAProcessManager::GetArrayInterface() const
    {
    return iCCALogIn->GetArrayInterface();
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetConversationInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAConversationPC* CCAProcessManager::GetConversationInterface( ) const
    {
    return iConversationPC;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetGroupInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAGroupPC* CCAProcessManager::GetGroupInterface( ) const
    {
    return iGroupPC;
    }
// -----------------------------------------------------------------------------
// CCAProcessManager::GetGroupUtilsInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAGroupUtilsPC* CCAProcessManager::GetGroupUtilsInterface( ) const
    {
    return iGroupUtilsPC;
    }


// -----------------------------------------------------------------------------
// CCAProcessManager::GetContactEditInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAContactEditPC* CCAProcessManager::GetContactEditInterface() const
    {
    return iCCAContactEditPC;
    }
// -----------------------------------------------------------------------------
// CCAProcessManager::GetSearchDataInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCASearchDataPC* CCAProcessManager::GetSearchDataInterface() const
    {
    return iDataInterfacePC;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetSearchInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCASearchInterfacePC* CCAProcessManager::GetSearchInterface() const
    {
    return iSearchInterfacePC;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetReactiveAuthInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAReactiveAuthPC* CCAProcessManager::GetReactiveAuthInterface() const
    {
    return iReactiveAuthPC;
    }
// -----------------------------------------------------------------------------
// CCAProcessManager::GetBlockingPCInterface
// -----------------------------------------------------------------------------
//

EXPORT_C MCABlockingPC* CCAProcessManager::GetBlockingInterface( ) const
    {
    return iCCABlockingPC;
    }


// -----------------------------------------------------------------------------
// CCAProcessManager::CreateInviteEditorPCL
// -----------------------------------------------------------------------------
//

EXPORT_C MCAInviteEditorPC* CCAProcessManager::CreateInviteEditorPCL()
    {

    MCAInviteEditorPC* inviteEditor = CCAInviteEditorPC::NewL( *iEngine->GetInvitationInterface(),
                                                               *CAPresenceManager::InstanceL() );
    //Ownership is transfered to the caller.
    return inviteEditor;
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::GetRecordedChatsPC
// -----------------------------------------------------------------------------
//

EXPORT_C MCARecordedChatsPC* CCAProcessManager::GetRecordedChatsPC() const
    {
    return iRecordedChatsPC;
    }
// -----------------------------------------------------------------------------
// CCAProcessManager::GetRecordedChatsArrayPC
// -----------------------------------------------------------------------------
//
EXPORT_C MCARecordedChatsArrayPC* CCAProcessManager::GetRecordedChatsArrayPC() const
    {
    return iRecordedChatsArrayPC;
    }
// -----------------------------------------------------------------------------
// CCAProcessManager::GetGlobalNotificationInterface
// -----------------------------------------------------------------------------
//
EXPORT_C MCAGlobalNotificationObserverPC* CCAProcessManager::GetGlobalNotificationInterface() const
    {
    return iGlobalNotificationObserverPC;
    }
// -----------------------------------------------------------------------------
// CCAProcessManager::GetRecordedChatsArrayPC
// -----------------------------------------------------------------------------
//
EXPORT_C MCAGroupPropertiesPC* CCAProcessManager::GetGroupPropertiesPC() const
    {
    return iGroupPropertiesPC;
    }

EXPORT_C MCAApplicationNGPC* CCAProcessManager::GetApplicationNGPC() const
    {
    return iApplicationNGPC;
    }

// -----------------------------------------------------------------------------
// Default Constructor
// -----------------------------------------------------------------------------
//
CCAProcessManager::CCAProcessManager()
    {

    }

// -----------------------------------------------------------------------------
// CCAProcessManager::FinalizeEngineConstructionL
// -----------------------------------------------------------------------------
//

EXPORT_C void CCAProcessManager::FinalizeEngineConstructionL()
    {
    iEngine->FinalizeEngineConstructionL();

    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//

EXPORT_C CCAProcessManager::~CCAProcessManager()
    {
    delete iApplicationNGPC;
    delete iGroupPropertiesPC;
    delete iGlobalNotificationObserverPC;
    delete iRecordedChatsPC;
    delete iRecordedChatsArrayPC;
    delete iCCALogIn;
    delete iSettingsPC;
    delete iConversationPC;
    delete iMainViewItemArray;
    delete iCCAContactEditPC;
    delete iCCABlockingPC;
    delete iDataInterfacePC;
    delete iSearchInterfacePC;
    delete iReactiveAuthPC;
    delete iInvitationsPC;
    delete iGroupPC;
    delete iGroupUtilsPC;
    delete iEngine;

    }

// -----------------------------------------------------------------------------
// CCAProcessManager::ConstructL
// -----------------------------------------------------------------------------
//

void CCAProcessManager::ConstructL()
    {


    iEngine = CCAEngine::NewL( KUidChatClient );

    iEngine->FinalizeEngineConstructionL();

    iCCALogIn = CCALoginPC::NewL( *iEngine );

    iSettingsPC = CCASettingsPC::NewL( *iEngine );

    iGlobalNotificationObserverPC =  CCAGlobalNotificationObserverPC::NewL(
                                         iEngine->ContactListModel(),
                                         iEngine->ChatInterface(),
                                         *( iEngine->GetInvitationInterface() ),
                                         *( iEngine->GetInternalSettingsInterface() ),
                                         iEngine->MessageUtils() );


    iConversationPC = CCAConversationPC::NewL( *iEngine );
    iCCAContactEditPC = CCAContactEditPC::NewL( *iSettingsPC );
    MCASearchInterface* searchInterface = iEngine->GetSearchInterface();
    MCABackgroundInterface* backgrdinterface = iEngine->BackgroundInterface();
    if ( searchInterface && backgrdinterface )
        {
        MCAGroupManagerInterface* groupMgrInterface = iEngine->GetGroupInterface();
        iSearchInterfacePC = CCASearchInterfacePC::NewL(
                                 searchInterface , *backgrdinterface, *groupMgrInterface );
        }


    if ( searchInterface )
        {
        iDataInterfacePC = CCASearchDataPC::NewL( searchInterface->SearchDataInterface() );
        }


    iCCABlockingPC = CCABlockingPC::NewL( *iEngine );

    iInvitationsPC = CCAInvitationPC::NewL( *iEngine, iEngine->ChatInterface() );

    //specific interfaces can be passed in NewL later
    iGroupPC 		= CCAGroupPC::NewL( iEngine->GetGroupInterface(),
                                   iEngine->ChatInterface(),
                                   iEngine->MessageUtils() );

    iGroupUtilsPC	= CCAGroupUtilsPC::NewL( *iEngine );

    MCALoggerHeadersInterface* loggerHeaderInterface = iEngine->GetLoggerHeadersInterface();
    iRecordedChatsArrayPC = CCARecordedChatsArrayPC::NewL( *loggerHeaderInterface );
    iRecordedChatsPC = CCARecordedChatsPC::NewL( *loggerHeaderInterface, *iRecordedChatsArrayPC );

    iReactiveAuthPC = CCAReactiveAuthPC::NewL();

    iGroupPropertiesPC = CCAGroupPropertiesPC::NewL( *iEngine->GetGroupInterface() );


    iApplicationNGPC = CCAApplicationNGPC::NewL( *iEngine );

    }

// -----------------------------------------------------------------------------
// CCAProcessManager::HandleContactDelete
// -----------------------------------------------------------------------------
//

void CCAProcessManager::HandleContactDelete( const TDesC& /*aContactId*/ )
    {
    //To be considered for CCAMainViewItemArray
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::HandleAddition
// -----------------------------------------------------------------------------
//

void CCAProcessManager::HandleAddition( MCAContactList& /*aList*/, MCAStoredContact& /*aContact*/ )
    {
    //To be considered for CCAMainViewItemArray
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::HandleChange
// -----------------------------------------------------------------------------
//

void CCAProcessManager::HandleChange( MCAContactList* /*aList*/, MCAStoredContact* /*aContact*/,
                                      TStorageManagerGlobals::TCAObserverEventType aEventType,
                                      TBool /*aUserIdChanged*/ )
    {
    if ( aEventType == TStorageManagerGlobals::EStorageEventOwnStatusChange ||
         aEventType == TStorageManagerGlobals::EStorageEventMultipleChanges )
        {
        //To be considered for CCAMainViewItemArray
        }
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::HandleMessageError
// -----------------------------------------------------------------------------
//

void CCAProcessManager::HandleMessageError( TInt aError, MCAMessage* /*aMessage*/ )
    {
    if ( aError == KErrNoMemory )
        {
        //To be considered later
        }
    }

// -----------------------------------------------------------------------------
// CCAProcessManager::InitializeEngineObserversL
// -----------------------------------------------------------------------------
//
void CCAProcessManager::InitializeEngineObserversL()
    {

    // engine is valid before this is called
    iEngine->ChatInterface().RegisterChatObserver( this );
    CCAStorageManagerFactory::ContactListInterfaceL()->AddObserverL( this );
    iEngine->MessageUtils().MessageErrorInformer().RegisterObserver( this );

    }

// -----------------------------------------------------------------------------
// CCAProcessManager::HandleInvitationEventL
// -----------------------------------------------------------------------------
//

void CCAProcessManager::HandleInvitationEventL( TInviteEventType /*aEvent*/,
                                                const MCAInvitation* /*aInvitation*/ )
    {
    //To be considered later
    }

// ---------------------------------------------------------
// CCAProcessManager::HandleInvitationResponse
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCAProcessManager::HandleInvitationResponse( TBool /*aAcceptance*/,
                                                  const TDesC& /*aUserId*/,
                                                  const TDesC& /*aGroupName*/,
                                                  const TDesC& /*aResponse*/ )
    {
    //To be considered later
    }


// ---------------------------------------------------------
// CCAProcessManager::HandleChatEvent
// (other items were commented in a header).
// ---------------------------------------------------------

void CCAProcessManager::HandleChatEvent( TChatEventType /*aEvent*/, MCAMessage* /*aMessage*/ )
    {
    //To be considered later
    }




// End of file
