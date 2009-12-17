/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence attribute transaction environment.
*
*/

// INCLUDE FILES
#include <E32Base.h>
#include <bamdesca.h>
#include "CPEngPresenceAttrTransactionEnv.h"
#include "CPEngPresenceNotificationHandler.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAttrModel2.h"
#include "MPEngPresenceAdvancedAttrModel2.h"
#include "MPEngXMLParser.h"
#include "RObjectArray.h"
#include "CPEngSessionSlotId.h"
#include "PEngAddressUtils.h"
#include "PEngAttrModelHelper.h"

#include "CPEngSubscribedUsers.h"
#include "CPEngSubscribedUser.h"
#include "CPEngOrphanNotificationQue.h"
#include "PresenceDebugPrint.h"


//CONSTANTS
const TInt KQuedOrphanNotifications = 10;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrTransactionEnv* CPEngPresenceAttrTransactionEnv::NewL(
    const CPEngSessionSlotId& aSlotId,
    MPEngPresenceAttrManager& aAttrMan )
    {
    CPEngPresenceAttrTransactionEnv* self = new ( ELeave ) CPEngPresenceAttrTransactionEnv
    ( aAttrMan );

    CleanupClosePushL( *self ); //Reference counted object
    self->ConstructL( aSlotId );
    CleanupStack::Pop(); //self

    return self;
    }


// Destructor
CPEngPresenceAttrTransactionEnv::~CPEngPresenceAttrTransactionEnv()
    {
    PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::~CPEngPresenceAttrTransactionEnv()" ) );
    delete iSubscribedUsers;
    delete iOrphanNotifications;
    iSessTracingAttribs.ResetAndDestroy();
    delete iDomain;

    if ( iAttributeManagerOwned )
        {
        iAttributeManager.Close();
        }
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::CPEngPresenceAttrTransactionEnv
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrTransactionEnv::CPEngPresenceAttrTransactionEnv( MPEngPresenceAttrManager& aAttrMan )
        : iAttributeManager( aAttrMan ),
        iAttributeManagerOwned( EFalse )
    {
    PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::CPEngPresenceAttrTransactionEnv()" ) );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::ConstructL( const CPEngSessionSlotId& aSlotId )
    {
    CPEngSessionSlotObject::ConstructL( aSlotId );

    //resolve transaction domain
    iDomain = PEngAddressUtils::WVDomain( aSlotId.UserId() ).AllocL();

    iSubscribedUsers = CPEngSubscribedUsers::NewL();
    iOrphanNotifications = CPEngOrphanNotificationQue::NewL( KQuedOrphanNotifications );

    iAttributeManagerOwned = ETrue;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::Close()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::Close()
    {
    CPEngSessionSlotObject::Close();
    }

// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::AddSubscriptionL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::AddSubscriptionL( const TDesC& aPresenceID )

    {
    PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::AddSubscriptionL() [%S]" ),
             &aPresenceID );

    __ASSERT_ALWAYS( aPresenceID.Length() > 0, User::Leave( KErrArgument ) );


    //Locate existing user subscription entry or create a new one
    const TInt uIndex = iSubscribedUsers->FindSubscribedUser( aPresenceID );
    if ( uIndex == KErrNotFound )
        {
        PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::AddSubscriptionL() - adding new" ) );

        CPEngSubscribedUser* userDetails = CPEngSubscribedUser::NewLC( aPresenceID );
        userDetails->AddSubscription(); //increase subscription count
        iSubscribedUsers->AddSubscribedUserL( userDetails );
        CleanupStack::Pop(); //userDetails

        DoInitializeAttributesToCacheL( aPresenceID );

        //New subscription added
        //Process possibly pending orphan presence notifications.
        DoProcessAndStoreOrphanNotificationsL();
        }

    else
        {
        CPEngSubscribedUser& userDetails = iSubscribedUsers->SubscribedUser( uIndex );

        PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::AddSubscriptionL() - Increasing existing, count %d" ),
                 userDetails.SubscriptionsCount() );


        userDetails.AddSubscription(); //increase subscription count
        }
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::RemoveSubscriptionL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::RemoveSubscriptionL( const TDesC& aPresenceID )
    {
    PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::RemoveSubscriptionL() [%S]" ),
             &aPresenceID );


    if ( aPresenceID.Length() == 0 )
        {
        return;
        }


    const TInt uIndex = iSubscribedUsers->FindSubscribedUser( aPresenceID );
    if ( uIndex != KErrNotFound )
        {
        CPEngSubscribedUser& userDetails = iSubscribedUsers->SubscribedUser( uIndex );
        userDetails.RemoveSubscription();

        PENG_DP( D_PENG_LIT( "CPEngPresenceAttrTransactionEnv::RemoveSubscription() - Subscription removed, Subscriptions left %d" ),
                 userDetails.SubscriptionsCount() );

        if ( userDetails.SubscriptionsCount() == 0 )
            {
            //no active subscription anymore for this user
            //==> all stored subscribed attributes can be deleted from storage
            DoClearAttributesFromCacheL( aPresenceID );

            //and actual subscription entry also
            iSubscribedUsers->DeleteSubscribeUser( uIndex );
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::SubscribedPresenceIDs()
// -----------------------------------------------------------------------------
//
const MDesCArray& CPEngPresenceAttrTransactionEnv::SubscribedPresenceIDs() const
    {
    return *iSubscribedUsers;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::SetSessionTracingAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::SetSessionTracingAttributesL(
    RPointerArray< MPEngPresenceAttrModel2 >& aAttributeModels  )
    {
    __ASSERT_DEBUG( !PEngAttrModelHelper::HasDuplicateTypes( aAttributeModels ),
                    User::Leave( KErrAlreadyExists ) );

    //Replace internal model array with given ones
    iSessTracingAttribs.ResetAndDestroy();
    PEngAttrModelHelper::MoveModelsL( aAttributeModels, iSessTracingAttribs );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::UpdateSessionTracingAttributesL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::UpdateSessionTracingAttributesL(
    RPointerArray< MPEngPresenceAttrModel2 >& aAttributeModels )
    {
    //go from end to start ==> remove is safe
    for ( TInt sourceIx = ( aAttributeModels.Count() - 1 ); sourceIx >= 0; sourceIx-- )
        {
        MPEngPresenceAttrModel2* sourceModel = aAttributeModels [ sourceIx ];

        const TInt tracedCount = iSessTracingAttribs.Count();
        for ( TInt tracedIx = 0; tracedIx < tracedCount; tracedIx++ )
            {
            MPEngPresenceAttrModel2* tracedModel = iSessTracingAttribs[ tracedIx ];
            if ( ( sourceModel->Type() == tracedModel->Type() ) &&
                 ( sourceModel->PresenceID() == tracedModel->PresenceID() ) )
                {
                //replace traced model with new one instance
                //and remove new instance from source array
                delete iSessTracingAttribs[ tracedIx ];
                iSessTracingAttribs[ tracedIx ] = sourceModel;
                aAttributeModels.Remove( sourceIx );
                break;
                }
            }
        }
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::HandleNetworkSessionStateChangeL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::HandleNetworkSessionStateChangeL(
    TBool aSessionOpen )
    {
    const TInt count = iSessTracingAttribs.Count();
    for ( TInt ii = 0; ii < count; ii++ )
        {
        MPEngPresenceAttrModel2* model = iSessTracingAttribs[ ii ];
        model->Advanced()->HandleNetworkSessionStateChangeL( aSessionOpen );
        }

    iAttributeManager.ForceStoreBatchL( iSessTracingAttribs );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::QueOrphanNotificationL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::QueOrphanNotificationL(
    const TDesC8& aPresenceBlock )
    {
    iOrphanNotifications->AddToQueL( aPresenceBlock );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::ProcessOrphanNotificationsL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::ProcessOrphanNotificationsL(
    RPointerArray< MPEngPresenceAttrModel2 >& aModels )
    {
    MPEngXMLParser* xmlParser = CreateXMLParserLC();
    const TDesC8* notification = iOrphanNotifications->GetFirst();

    while ( notification )
        {
        TBool notificationMached =
            CPEngPresenceNotificationHandler::ProcessNotifyBlockL( *notification,
                                                                   *iDomain,
                                                                   *iSubscribedUsers,
                                                                   iAttributeManager,
                                                                   *xmlParser,
                                                                   aModels );

        //If notification matched to subscribed user => discard the cache entry
        //Otherwise proceed to next cache entry
        if ( notificationMached )
            {
            notification = iOrphanNotifications->DeleteCurrentAndGetNext();
            }
        else
            {
            notification = iOrphanNotifications->GetNext();
            }
        }

    CleanupStack::PopAndDestroy(); //xmlParser
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::DoInitializeAttributesToCacheL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::DoInitializeAttributesToCacheL(
    const TDesC& aPresenceID )
    {
    TArray<TUint32> attributes = iAttributeManager.KnownAttributeTypes();
    const TInt count = attributes.Count();

    RObjectArray<MPEngPresenceAttrModel2> models;
    CleanupClosePushL( models );

    for ( TInt ii = 0; ii < count; ii++ )
        {
        TUint32 attribute = attributes[ ii ];
        MPEngPresenceAttrModel2* model = NULL;
        TInt err = iAttributeManager.LoadAttributeLC( model,
                                                      aPresenceID,
                                                      attribute,
                                                      EPEngStorableModel );
        if ( err == KErrNotFound )
            {
            models.AppendL( model );
            CleanupStack::Pop(); //model
            }
        else
            {
            CleanupStack::PopAndDestroy(); //model
            }
        }

    iAttributeManager.ForceStoreBatchL( models );
    CleanupStack::PopAndDestroy(); //models
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::DoClearAttributesFromCacheL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::DoClearAttributesFromCacheL(
    const TDesC& aPresenceID )
    {
    TArray<TUint32> attributes = iAttributeManager.KnownAttributeTypes();
    const TInt count = attributes.Count();

    RObjectArray<MPEngPresenceAttrModel2> models;
    CleanupClosePushL( models );

    for ( TInt ii = 0; ii < count; ii++ )
        {
        TUint32 attribute = attributes[ ii ];
        MPEngPresenceAttrModel2* model = NULL;
        TInt err = iAttributeManager.LoadAttributeLC( model,
                                                      aPresenceID,
                                                      attribute,
                                                      EPEngStorableModel );
        if ( err == KErrNone )
            {
            models.AppendL( model );
            CleanupStack::Pop(); //model
            }
        else
            {
            CleanupStack::PopAndDestroy(); //model
            }
        }

    iAttributeManager.ForceRemoveFromStoreBatchL( models );
    CleanupStack::PopAndDestroy(); //models
    }




// -----------------------------------------------------------------------------
// CPEngPresenceAttrTransactionEnv::DoProcessAndStoreOrphanNotificationsL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrTransactionEnv::DoProcessAndStoreOrphanNotificationsL()
    {
    RObjectArray<MPEngPresenceAttrModel2> models;
    CleanupClosePushL( models );

    ProcessOrphanNotificationsL( models );

    iAttributeManager.ForceStoreBatchL( models );
    CleanupStack::PopAndDestroy(); //models
    }


//  End of File

