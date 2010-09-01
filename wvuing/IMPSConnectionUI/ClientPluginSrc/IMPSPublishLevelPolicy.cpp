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
* Description: Presence Publish Level Policy which offers publish level changing and initializing
*
*/

// INCLUDE FILES
#include  <E32Base.h>

#include  "IMPSPublishLevelPolicy.h"
#include  "MIMPSSharedData.h"
#include  "CIMPSSharedDataFactory.h"
#include  "IMPSUIDDefs.h"
#include  "impspresenceconnectionuiconstsng.h"

// Attribute lists support
#include <CPEngAttributeListStore2.h>
#include <MPEngAttributeList2.h>
#include <MPEngPresenceAttrTypeProperties2.h>
#include <MPEngContactList2.h>
#include <CPEngNWSessionSlotID2.h>
#include <PEngWVPresenceAttributes2.h>

// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::InitializePublishLevelL()
// Public
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::InitializePublishLevelL( CPEngNWSessionSlotID2& aNWSessionSlotID )
    {

    MIMPSSharedData* sharedData =
        CIMPSSharedDataFactory::CreatePermanentKeyHandlerL(
            NULL,
            KIMPSPresenceKeyUid );

    TInt newPublishLevel( 0 );
    if ( KErrNotFound == sharedData->GetIntKey( EIMPSSharedKeysPECAppKeyNamePublishing,
                                                newPublishLevel ) )
        {
        // shared data key does not exists, it means we are probably running first time
        // create key and set it to the defaul value EPECDopPrivateAndPublic
        sharedData->SetIntKey( EIMPSSharedKeysPECAppKeyNamePublishing, EPECDopPrivateAndPublic );
        newPublishLevel = EPECDopPrivateAndPublic;
        }

    delete sharedData;
    // update attribute lists
    IMPSPublishLevelPolicy::UpdateAttributeListsL( newPublishLevel, aNWSessionSlotID );
    }


// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::UpdateAttributeListsL()
// Static updater
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::UpdateAttributeListsL( TInt aPublishLevel,
                                                    CPEngNWSessionSlotID2& aNWSessionSlotID )
    {

    // set new settings of the attribute lists
    CPEngAttributeListStore2* attrListFactory =
        CPEngAttributeListStore2::NewLC( aNWSessionSlotID );
    switch ( aPublishLevel )
        {
        case EPECDopPrivateAndPublic:
            {
            SetPrivatePublicPublishingL( *attrListFactory );
            break;
            }

        case EPECDopOnlyPrivate:
            {
            SetOnlyPrivatePublishingL( *attrListFactory );
            break;
            }

        case EPECDopOnlyPublic:
            {
            SetOnlyPublicPublishingL( *attrListFactory );
            break;
            }

        case EPECDopOff:
            {
            SetPublishOffL( *attrListFactory );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }

    CleanupStack::PopAndDestroy( attrListFactory ); // attributeFactory

    }


// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::SetPrivatePresenceL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetPrivatePresenceL(
    MPEngAttributeList2& aAttrListModel )
    {
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrUserAvailability );
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrOnlineStatus );
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrClientInfo );
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrStatusText );
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrStatusContent );
    }

// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::SetPublicPresenceL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetPublicPresenceL(
    MPEngAttributeList2& aAttrListModel )
    {
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrUserAvailability );
    aAttrListModel.AddPresenceAttributeL( KUidPrAttrOnlineStatus );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrClientInfo );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrStatusText );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrStatusContent );
    }

// -----------------------------------------------------------------------------
// PAppPublishLevelPolicy::SetNoPresence()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetNoPresence(
    MPEngAttributeList2& aAttrListModel )
    {
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrUserAvailability );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrOnlineStatus );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrClientInfo );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrStatusText );
    aAttrListModel.RemovePresenceAttribute( KUidPrAttrStatusContent );
    }


// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::SetPrivatePublicPublishingL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetPrivatePublicPublishingL(
    CPEngAttributeListStore2& aAttributeListFactory )
    {

    // contacts of private contact list, get all presence attributes
    // default attribute lists is with default attributes
    // block lists is empty

    // default attr list
    UpdateDefaultAttrListL( aAttributeListFactory, SetPublicPresenceL );

    // block list
    UpdateAttributeListL( aAttributeListFactory, KIMPSBlockList, SetNoPresence );

    // private list
    UpdateAttributeListL( aAttributeListFactory, KIMPSPrivateList, SetPrivatePresenceL );

    }



// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::SetOnlyPrivatePublishingL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetOnlyPrivatePublishingL(
    CPEngAttributeListStore2& aAttributeListFactory )
    {

    // contacts of private contact list, get all presence attributes
    // default attribute lists is empty, block lists is empty

    // default attr list
    UpdateDefaultAttrListL( aAttributeListFactory, SetNoPresence );

    // block list
    UpdateAttributeListL( aAttributeListFactory, KIMPSBlockList, SetNoPresence );

    // private list
    UpdateAttributeListL( aAttributeListFactory, KIMPSPrivateList, SetPrivatePresenceL );
    }



// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::SetOnlyPublicPublishingL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetOnlyPublicPublishingL(
    CPEngAttributeListStore2& aAttributeListFactory )
    {
    // contacts of private contact list, get only public attributes
    // default attribute lists is set to public attributes, block lists is empty

    // default attr list
    UpdateDefaultAttrListL( aAttributeListFactory, SetPublicPresenceL );

    // block list
    UpdateAttributeListL( aAttributeListFactory, KIMPSBlockList, SetNoPresence );

    // private list
    UpdateAttributeListL( aAttributeListFactory, KIMPSPrivateList, SetPublicPresenceL );
    }



// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::SetPublishOffL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::SetPublishOffL( CPEngAttributeListStore2& aAttributeListFactory )
    {
    // all contacts shouldn't receive any presence attributes => all attribute lists without presence attributes

    // default attribut elist
    UpdateDefaultAttrListL( aAttributeListFactory, SetNoPresence );

    // private list
    UpdateAttributeListL( aAttributeListFactory, KIMPSPrivateList, SetNoPresence );

    // block list
    UpdateAttributeListL( aAttributeListFactory, KIMPSBlockList, SetNoPresence );
    }


// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::UpdateAttributeListL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::UpdateAttributeListL(
    CPEngAttributeListStore2& aAttrLstStore,
    const TDesC& aContactList,
    TSetPresenceL aPresenceCall )
    {
    MPEngAttributeList2* attrListModel = NULL;
    TRAPD( e , attrListModel =
               aAttrLstStore.GetAttributeListForContactListL( aContactList,
                                                              EPEngLocalAttributeLists ) );
    // if attribute list does not exists, create it
    if ( ( e != KErrNone ) && ( e != KErrNotFound ) )
        {
        User::Leave( e );
        }
    if ( e == KErrNotFound )
        {
        // have a look in network attributes
        TRAP( e , attrListModel =
                  aAttrLstStore.GetAttributeListForContactListL( aContactList,
                                                                 EPEngNetworkAttributeLists ) );
        if ( ( e != KErrNone ) && ( e != KErrNotFound ) )
            {
            User::Leave( e );
            }
        if ( e == KErrNotFound )
            {
            attrListModel = aAttrLstStore.CreateEmptyAttributeListL();
            }
        }

    CleanupClosePushL( *attrListModel );
    ( *aPresenceCall )( *attrListModel );
    aAttrLstStore.AttachAttributeListToContactListL( aContactList, *attrListModel );
    CleanupStack::PopAndDestroy(); // attrListModel
    }

// -----------------------------------------------------------------------------
// IMPSPublishLevelPolicy::UpdateDefaultAttrListL()
// Value handler
// -----------------------------------------------------------------------------
//
void IMPSPublishLevelPolicy::UpdateDefaultAttrListL(
    CPEngAttributeListStore2& aAttrLstStore,
    TSetPresenceL aPresenceCall )
    {
    MPEngAttributeList2* attrListModel = NULL;
    TRAPD( e , attrListModel = aAttrLstStore.GetDefaultAttributeListL( EPEngLocalAttributeLists ) );
    // if attribute list does not exists, create it
    if ( ( e != KErrNone ) && ( e != KErrNotFound ) )
        {
        User::Leave( e );
        }
    if ( e == KErrNotFound )
        {
        // have a look in network attributes
        TRAPD( e , attrListModel =
                   aAttrLstStore.GetDefaultAttributeListL( EPEngNetworkAttributeLists ) );
        if ( ( e != KErrNone ) && ( e != KErrNotFound ) )
            {
            User::Leave( e );
            }
        if ( e == KErrNotFound )
            {
            attrListModel = aAttrLstStore.CreateEmptyAttributeListL();
            }
        }
    CleanupClosePushL( *attrListModel );
    ( *aPresenceCall )( *attrListModel );
    aAttrLstStore.SetAsDefaultAttributeListL( *attrListModel );
    CleanupStack::PopAndDestroy(); // attrListModel
    }


//  End of File
