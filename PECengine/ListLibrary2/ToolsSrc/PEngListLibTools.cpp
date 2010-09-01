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
* Description:  Various tools for List Library
*
*/


//  INCLUDES
#include "PEngListLibTools.h"
#include "PEngContactIdsTools.h"

#include "CPEngContactListModBase.h"
#include "CPEngContactListModItemContainer.h"
#include "CPEngContactListModChangeMonitor.h"
#include "CPEngContactListSettings.h"
#include "MPEngContactListTransactionManager.h"
#include "CPEngTransactionStatus.h"
#include "PEngXMLTags.h"
#include "MPEngXMLParser.h"
#include "MPEngXMLSerializer.h"
#include "PEngAttrLibFactory.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAttrTransactionEnv.h"
#include "PEngWVPresenceErrors2.h"
#include "PresenceDebugPrint.h"



// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DoIsContactUpdated()
//
// Checks if:
// - the contact has been updated or is new
// - or its Nick name was updated
// - or contact was added to the contact list
// - or server altered contact id, so its length changed
//
// @param aContact Contact item container
// @param aNewContactId Contact Id as server returned it
// @param aNewNickName New nick name returned by server
// -----------------------------------------------------------------------------
//
inline TBool DoIsContactUpdated( CPEngContactListModItemContainer& aContact,
                                 const TDesC& aNewContactId,
                                 HBufC* aNewNickName )
    {
    const HBufC* currentNick = aContact.CurrentNick();

    TBool newValid( aNewNickName && aNewNickName->Length() );
    TBool curentValid( currentNick && currentNick->Length() );
    if (
        ( !aContact.RefActive( CPEngContactListModItemContainer::ENetworkRef ) )
        ||
        ( aNewContactId.Length() != aContact.Id().Length() )
        ||
        (
            ( newValid
              ||
              curentValid
            )
            &&
            ( !newValid
              ||
              !curentValid
              ||
              ( KErrNone != aNewNickName->Compare( *currentNick ) )
            )
        )
    )
        {
        return ETrue;
        }
    return EFalse;

    }

// -----------------------------------------------------------------------------
// DoInsertContactLX()
// Inserts contact item to array of parsed contacts
// aContactId and aNickName has to be pushed on the top of the clean up stack.
// Order of those two items on clean up stack does not matter, as far as they
// are on top.
// Ownership of the aContactId and aNickName are tranfered to the function.
// Function returns EFalse if contact was existing on server before
// and ETrue if contact was just created on the server
// -----------------------------------------------------------------------------
//
TBool DoInsertContactLX( RPointerArray<CPEngContactListModItemContainer>& aParsedContacts,
                         RPointerArray<CPEngContactListModItemContainer>& aServerOrderContacts,
                         const RPointerArray<CPEngContactListModItemContainer>& aOldServerOrder,
                         CPEngContactListModBase& aContactList,
                         TBool aUpdateNickname,
                         HBufC* aContactId,
                         HBufC* aNickName,
                         TInt& aServerIndex,
                         TInt& aOffset,
                         TInt aOldServerCount )
    {
    // get position for insertion
    TInt pos( KErrNotFound );

    if ( KErrNone == aContactList.DoFindContact( aParsedContacts,
                                                 *aContactId,
                                                 pos,
                                                 ETrue ) )
        {
        // contact is twice in the response, server is blabbering
        // Such a contact was already parsed and inserted, ignore it
        CleanupStack::PopAndDestroy( 2 ); // aNickName, aContactId
        return EFalse;
        }


    // does contact already exists?
    // look firs in the server ordered array
    CPEngContactListModItemContainer* cnt = NULL;
    TInt serverIndex( aServerIndex + aOffset );
    if ( serverIndex < aOldServerCount )
        {
        if ( KErrNone == NContactIdsTools::CompareContactIds( aOldServerOrder[ serverIndex ]->Id(),
                                                              *aContactId ) )
            {
            cnt = aOldServerOrder[ serverIndex ];
            }
        else
            {
            serverIndex++;
            // look one ahead, in case some contact was deleted
            if ( ( serverIndex < aOldServerCount ) &&
                 ( KErrNone == NContactIdsTools::CompareContactIds( aOldServerOrder[ serverIndex ]->Id(),
                                                                    *aContactId ) ) )
                {
                // some contact was removed from server array, update offset
                cnt = aOldServerOrder[ serverIndex ];
                aOffset++;
                }
            }
        }


    if ( !cnt )
        {
        cnt = aContactList.FindContactInAll( *aContactId );
        // contact was added to the contact, update offset
        aOffset--;
        }


    if ( cnt )
        {
        // add to the array
        aParsedContacts.InsertL( cnt, pos );
        cnt->Open();                                    // CSI: 65 #

        // check if nick name has changed
        if ( DoIsContactUpdated( *cnt, *aContactId, aNickName ) )
            {
            aContactList.ChangeMonitor().InsertAddedContactIdL( *aContactId );
            // current nick will be updated out of this condition with nick parsed
            // from server response, update New Nick of the Conatct Item container
            if ( aUpdateNickname )
                {
                cnt->RollBackNickname();
                }
            else
                {
                cnt->AdoptCurrentNickAsNew();
                }
            }

        // update contact id as it is given by the network server
        cnt->SetCurrentNickname( aNickName );
        cnt->UpdateContactId( aContactId );
        CleanupStack::Pop( 2 ); // nickName, aContactId
        aServerOrderContacts.AppendL( cnt );
        cnt->SetServerIndex( aServerIndex );
        aServerIndex++;
        return !aContactList.ContactExistsOnServer( *cnt );
        }
    else
        {
        CPEngContactListModItemContainer* newCnt =
            CPEngContactListModItemContainer::NewLC(
                aContactList,
                *aContactId,
                aNickName );
        aParsedContacts.InsertL( newCnt, pos );
        CleanupStack::Pop(); // newCnt
        aContactList.ChangeMonitor().InsertAddedContactIdL( *aContactId );
        CleanupStack::PopAndDestroy( 2 ); // contactId, nickName
        aServerOrderContacts.AppendL( newCnt );
        newCnt->SetServerIndex( aServerIndex );
        aServerIndex++;
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// DoFillChangeMonitorOfRemovedIdsL()
// Fills the monitor of removed contact Ids.
// -----------------------------------------------------------------------------
//
void DoFillChangeMonitorOfRemovedIdsL(
    CPEngContactListModBase& aContactList,
    const RPointerArray<CPEngContactListModItemContainer>& aNewContacts )
    {
    TInt count( aContactList.Count( EPEngCntListNetworkView ) );
    for ( TInt x( 0 ) ; x < count ; ++x )
        {
        CPEngContactListModItemContainer* cnt =
            static_cast<CPEngContactListModItemContainer*>
            ( &( aContactList.ContactItem(
                     x,
                     EPEngCntListNetworkView ) ) );

        if ( KErrNotFound == aNewContacts.Find( cnt ) )
            {
            aContactList.ChangeMonitor().InsertRemovedContactIdL( cnt->Id() );
            }
        }
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NListLibTools::LeaveIfMajorErrorL()
// -----------------------------------------------------------------------------
//
void NListLibTools::LeaveIfMajorErrorL( TInt aError )
    {
    if ( ( aError < 0 ) &&
         ( aError != KErrAlreadyExists ) )
        {
        User::Leave( aError );
        }
    }



// -----------------------------------------------------------------------------
// NListLibTools::ParseContactListNickListL()
// -----------------------------------------------------------------------------
//
TBool NListLibTools::ParseContactListNickListL(
    const TDesC8& aResponse,
    RPointerArray<CPEngContactListModItemContainer>& aParsedContacts,
    CPEngContactListModBase& aContactList,
    MPEngXMLParser& aXMLparser1,
    MPEngXMLParser& aXMLparser2,
    TBool aUpdateNickname,
    TInt* aNewContactCount /* NULL */ )
    {
    // check if there is even nick List
    PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KNickList)-Start" ) );
    if ( !aXMLparser1.DecodeL( aResponse, KNickList, ETrue ) )
        {
        return EFalse;
        }
    PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KNickList)-Done" ) );

    TPtrC8 nickListBlock( aXMLparser1.ResultL( ) );
    TInt newContactCount( 0 );

    RPointerArray<CPEngContactListModItemContainer> serverOrderedContacts;
    CleanupClosePushL( serverOrderedContacts );

    TInt serverOrderIndex( 0 );
    TInt offset( 0 );
    const RPointerArray<CPEngContactListModItemContainer>& oldServerOrder =
        aContactList.ServerOrderedContacts();
    TInt oldServerCount( oldServerOrder.Count() );


    // First parse nicks with nick names
    PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KNickName)-Start" ) );
    if ( aXMLparser1.DecodeL( nickListBlock, KNickName, ETrue ) )
        {
        PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KNickName)-Done" ) );

        // Assume server ordered it alphabeticaly, used for speed up
        for ( TInt x ( aXMLparser1.Count() - 1 ) ; x >= 0 ; --x )
            {
            HBufC* nickName = NULL;
            HBufC* contactId = NULL;

            if ( aXMLparser2.DecodeL( aXMLparser1.ResultL( x ),
                                      KUserIDXMLTag,
                                      EFalse ) )
                {
                // Get contact Id, does not check if there is wv: prefix
                contactId = aXMLparser2.ResultAsUnicodeTextL();
                CleanupStack::PushL( contactId );
                }
            else
                {
                // XML package is not valid
                User::Leave( KErrGeneral );
                }

            if ( aXMLparser2.DecodeL( aXMLparser1.ResultL( x ),
                                      KName,
                                      EFalse ) )
                {
                nickName = aXMLparser2.ResultAsUnicodeTextL();
                }

            // both contactId and nickName has to be on cleanup stack
            // before calling DoInsertContactLX, ownership is transfered
            CleanupStack::PushL( nickName );
            newContactCount += DoInsertContactLX( aParsedContacts,
                                                  serverOrderedContacts,
                                                  oldServerOrder,
                                                  aContactList,
                                                  aUpdateNickname,
                                                  contactId,
                                                  nickName,
                                                  serverOrderIndex,
                                                  offset,
                                                  oldServerCount );
            }
        PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KNickName)-End" ) );
        }


    // now parse all nicks, to find which were without nicks
    PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KUserIDXMLTag)-Start" ) );
    if ( aXMLparser1.DecodeL( nickListBlock, KUserIDXMLTag, EFalse ) )
        {
        PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KUserIDXMLTag)-Done" ) );

        TInt count ( aXMLparser1.Count() );
        TInt alreadyParsed( serverOrderedContacts.Count() );

        for ( TInt x ( aXMLparser1.Count() - 1 ) ; x >= 0 ; --x )
            {
            HBufC* nickName = NULL;
            HBufC* contactId = aXMLparser1.ResultAsUnicodeTextL( x );

            // try to add contact if it does not match one from
            // already parsed contacts, we can use plain compare->faster
            if ( alreadyParsed &&
                 ( KErrNone == serverOrderedContacts[ 0 ]->Id().Compare( *contactId ) ) )
                {
                delete contactId;
                serverOrderedContacts.Remove( 0 );
                alreadyParsed--;
                continue;
                }

            CleanupStack::PushL( contactId );
            CleanupStack::PushL( nickName );

            // both contactId and nickName has to be on cleanup stack
            // before calling DoInsertContactLX, ownership is transfered
            newContactCount += DoInsertContactLX(
                                   aParsedContacts,
                                   serverOrderedContacts,
                                   oldServerOrder,
                                   aContactList,
                                   aUpdateNickname,
                                   contactId,
                                   nickName,
                                   serverOrderIndex,
                                   offset,
                                   oldServerCount );
            }

        PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::Decode(KUserIDXMLTag)-End" ) );
        }

    CleanupStack::PopAndDestroy(); // serverOrderedContacts

    if ( aNewContactCount )
        {
        *aNewContactCount += newContactCount;
        }


    // Fill monitor of removed contacts
    DoFillChangeMonitorOfRemovedIdsL( aContactList, aParsedContacts );
    PENG_DP( D_PENG_LIT( "NListLibTools::ParseContactListNickListL::-End" ) );
    return ETrue;
    }




// -----------------------------------------------------------------------------
// NListLibTools::ParseContactListIDsL()
// -----------------------------------------------------------------------------
//
TBool NListLibTools::ParseContactListIDsL(
    const TDesC8& aResponse,
    RPointerArray<CPEngContactListModItemContainer>& aParsedContacts,
    CPEngContactListModBase& aContactList,
    MPEngXMLParser& aXMLparser1,
    TBool aUpdateNickname,
    TInt* aNewContactCount /* NULL */ )
    {
    TInt newContactCount( 0 );

    RPointerArray<CPEngContactListModItemContainer> serverOrderedContacts;
    CleanupClosePushL( serverOrderedContacts );

    // Parse the list
    if ( aXMLparser1.DecodeL( aResponse, KUserIDXMLTag, EFalse ) )
        {
        HBufC* nickName = NULL;
        HBufC* contactId = NULL;
        TInt serverOrderIndex( 0 );
        TInt offset( 0 );


        const RPointerArray<CPEngContactListModItemContainer>& oldServerOrder =
            aContactList.ServerOrderedContacts();
        TInt oldServerCount( oldServerOrder.Count() );


        for ( TInt x ( aXMLparser1.Count() - 1 ) ; x >= 0 ; --x )
            {
            // Get contact Id
            contactId = aXMLparser1.ResultAsUnicodeTextL( x );
            CleanupStack::PushL( contactId );
            CleanupStack::PushL( nickName );

            // try to add contact
            // both contactId and nickName has to be on cleanup stack
            // before calling DoInsertContactLX, ownership is transfered
            newContactCount += DoInsertContactLX( aParsedContacts,
                                                  serverOrderedContacts,
                                                  oldServerOrder,
                                                  aContactList,
                                                  aUpdateNickname,
                                                  contactId,
                                                  nickName,
                                                  serverOrderIndex,
                                                  offset,
                                                  oldServerCount );
            }
        }

    CleanupStack::PopAndDestroy(); // serverOrderedContacts

    if ( aNewContactCount )
        {
        *aNewContactCount += newContactCount;
        }

    //Fill monitor of removed contacts
    DoFillChangeMonitorOfRemovedIdsL( aContactList, aParsedContacts );

    return ETrue;
    }


// -----------------------------------------------------------------------------
// NListLibTools::ParseContactListPropertiesL()
// -----------------------------------------------------------------------------
//
TBool NListLibTools::ParseContactListPropertiesL( const TDesC8& aResponse,
                                                  CPEngContactListSettings& aSettings,
                                                  MPEngXMLParser& aXMLparser1,
                                                  MPEngXMLParser& aXMLparser2 )
    {
    // get a property - Disp name Nick Name tag - UserName Tag
    TBool updateNeeded( EFalse );
    if ( aXMLparser1.DecodeL( aResponse , KProperty, ETrue ) )
        {
        TInt count ( aXMLparser1.Count() );
        for ( TInt y( 0 ) ; y < count ; y++ )
            {
            if ( aXMLparser2.DecodeL( aXMLparser1.ResultL( y ),
                                      KName,
                                      EFalse )
               )
                {
                TBool syncMaster( aSettings.Property(
                                      KPEngCntLstSyncMaster,
                                      KPEngCntLstPropertyNativePermanent ) );

                // name tag is there, get name and setting
                if ( aXMLparser2.ResultL().CompareF( KDispName ) == KErrNone  )
                    {
                    // good here we have a disp name - get it (value)
                    if ( aXMLparser2.DecodeL( aXMLparser1.ResultL( y ),
                                              KValue,
                                              EFalse ) )
                        {
                        HBufC* dispName = aXMLparser2.ResultAsUnicodeTextL();
                        // check if we need some update
                        if ( syncMaster )
                            {
                            CleanupStack::PushL( dispName );
                            // check if disp name is same as at local
                            updateNeeded = ( KErrNone !=
                                             aSettings.DispName().Compare(
                                                 *dispName ) );
                            CleanupStack::PopAndDestroy( dispName );
                            }
                        else
                            {
                            // slave for sync, just store it, safe like this
                            aSettings.PushUpdateDisplayNameL( dispName );
                            }
                        }
                    continue;
                    }
                // parse default flag
                if ( aXMLparser2.ResultL().CompareF( KDefault ) == KErrNone  )
                    {
                    // OK that is settings of the default list
                    // good here we have a disp name - get it (value)
                    if ( aXMLparser2.DecodeL( aXMLparser1.ResultL( y ),
                                              KValue,
                                              EFalse ) )
                        {
                        TBool defValue( aXMLparser2.ResultL().CompareF(
                                            KXMLValueTrue ) == KErrNone );

                        if ( syncMaster )
                            {
                            updateNeeded = ( defValue != aSettings.Property(
                                                 KPEngCntLstDefault,
                                                 KPEngCntLstPropertyNativePermanent ) );
                            }
                        else
                            {
                            aSettings.SetPropertyL(
                                KPEngCntLstDefault,
                                KPEngCntLstPropertyNativePermanent,
                                defValue );
                            }
                        }
                    }
                }
            }
        }

    return updateNeeded;
    }


// -----------------------------------------------------------------------------
// NListLibTools::RemoveBadContactsL()
// -----------------------------------------------------------------------------
//
void NListLibTools::RemoveBadContactsL(
    const CPEngTransactionStatus& aTransactionStatus,
    CPEngContactListModBase& aContactList )
    {

    TInt count ( aTransactionStatus.DetailedResultCount() );
    for ( TInt x ( 0 ) ; x < count ; ++x )
        {
        const MPEngDetailedResultEntry2& detailedResult =
            aTransactionStatus.DetailedResult( x );

        TPtrC badContactId;
        if ( KErrNone == detailedResult.GetDetailedDesc( badContactId,
                                                         EPEngDTPresenceID ) )
            {
            aContactList.RemoveBadContactL( badContactId );
            }
        }
    }


// -----------------------------------------------------------------------------
// NListLibTools::MarkForRemoveBadContactsL()
// -----------------------------------------------------------------------------
//
void NListLibTools::MarkForRemoveBadContactsL(
    const CPEngTransactionStatus& aTransactionStatus,
    CPEngContactListModBase& aContactList )
    {
    TInt count ( aTransactionStatus.DetailedResultCount() );
    for ( TInt i ( 0 ) ; i < count ; i++ )
        {
        const MPEngDetailedResultEntry2& detailedResult =
            aTransactionStatus.DetailedResult( i );

        if ( detailedResult.Error() == KPEngNwErrUnknownUser )
            {
            TPtrC badContact;
            detailedResult.GetDetailedDesc( badContact,
                                            EPEngDTPresenceID );

            // set contact to be removed at next transaction
            aContactList.RemoveContactL( badContact );
            }
        }
    }


// -----------------------------------------------------------------------------
// NListLibTools::HarmonizeDefaultContactListSettingsL()
// -----------------------------------------------------------------------------
//
void NListLibTools::HarmonizeDefaultContactListSettingsL(
    CPEngContactListSettings& aCntListSettings,
    MPEngContactListTransactionManager& aManager )
    {
    // update default list settings if needed
    if ( aCntListSettings.Property( KPEngNewDefaultContactList,
                                    KPEngCntLstPropertyNativeCached ) )
        {
        // original default contact list
        CPEngContactListSettings* defaultCntList =
            aManager.DefaultCntListSettingsOrNull();

        // update old default contact list
        if ( defaultCntList )
            {
            defaultCntList->DeletePropertyL(
                KPEngNewDefaultContactList,
                KPEngCntLstPropertyNativeCached );

            defaultCntList->DeletePropertyL(
                KPEngCntLstDefault,
                KPEngCntLstPropertyNativePermanent );
            }

        // new default contact list
        aCntListSettings.DeletePropertyL( KPEngNewDefaultContactList,
                                          KPEngCntLstPropertyNativeCached );

        aCntListSettings.SetPropertyL( KPEngCntLstDefault,
                                       KPEngCntLstPropertyNativePermanent,
                                       ETrue );
        }
    }


// -----------------------------------------------------------------------------
// NListLibTools::ActivatePresenceAttributesL()
// -----------------------------------------------------------------------------
//
void NListLibTools::ActivatePresenceAttributesL(
    const MDesCArray& aContacts,
    const CPEngSessionSlotId& aSessionSlotId )
    {

    MPEngPresenceAttrTransactionEnv* attributeManager =
        PEngAttrLibFactory::AttributeTransEnvInstanceLC( aSessionSlotId );
    TInt count( aContacts.MdcaCount() );
    for ( TInt x( 0 ); x < count ; x++ )
        {
        attributeManager->AddSubscriptionL( aContacts.MdcaPoint( x ) );
        }
    CleanupStack::PopAndDestroy(); // attributeManager
    }


// -----------------------------------------------------------------------------
// NListLibTools::DeactivatePresenceAttributesL()
// -----------------------------------------------------------------------------
//
void NListLibTools::DeactivatePresenceAttributesL(
    const MDesCArray& aContacts,
    const CPEngSessionSlotId& aSessionSlotId )
    {
    MPEngPresenceAttrTransactionEnv* attributeManager =
        PEngAttrLibFactory::AttributeTransEnvInstanceLC( aSessionSlotId );

    const TInt count( aContacts.MdcaCount() );
    for ( TInt x( 0 ); x < count ; x++ )
        {
        attributeManager->RemoveSubscriptionL( aContacts.MdcaPoint( x ) );
        }
    CleanupStack::PopAndDestroy(); // attributeManager
    }


// -----------------------------------------------------------------------------
// NListLibTools::AppendTransactionContentTagXmlL()
// -----------------------------------------------------------------------------
//
void NListLibTools::AppendTransactionContentTagXmlL( MPEngXMLSerializer& aXMLSerializer,
                                                     TPEngWVCspVersion& aCSPVersion )
    {
    if ( aCSPVersion == EWVCspV11 )
        {
        aXMLSerializer.StartTagL(  KTransactionContent );
        aXMLSerializer.AttributeL( KXmlXmlns, KTransactionContentNS );
        }
    else
        {
        aXMLSerializer.StartTagL( KTransactionContent );
        aXMLSerializer.AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
        }
    }



// -----------------------------------------------------------------------------
// NListLibTools::AppendContactListNameXmlL()
// -----------------------------------------------------------------------------
//
void NListLibTools::AppendContactListNameXmlL( MPEngXMLSerializer& aXmlSerializer,
                                               const CPEngContactListSettings& aSettings )
    {
    // <ContactList>
    // Contact list name value
    // </ContactList>
    aXmlSerializer.StartTagL( KContactList
                            ).WvAddressL( aSettings.ServerName()
                                        ).EndTagL( KContactList );
    }


// -----------------------------------------------------------------------------
// NListLibTools::AppendAttributesForSubscriptionXmlL()
// -----------------------------------------------------------------------------
//
void NListLibTools::AppendAttributesForSubscriptionXmlL(
    const RArray<TUint32> aAttributes,
    MPEngXMLSerializer& aXmlSerializer,
    const CPEngSessionSlotId& aSessionSlotId,
    TPEngWVCspVersion& aCSPVersion )
    {
    // array of attributes for subscribing can be empty
    TInt count ( aAttributes.Count() );

    // <PresenceSubList
    aXmlSerializer.StartTagL( KPresenceSubList );

    // if count is zero append just empty PresenceSubList tag
    if ( count == 0 )
        {
        if ( aCSPVersion == EWVCspV11 )
            {
            // xmlns="http://www.wirelessvillage.org/PA1.1"
            aXmlSerializer.AttributeL( KXmlXmlns, KPresenceSubListAttributesNS );
            }
        else
            {
            // xmlns="http://www.openmobilealliance.org/DTD/WV-PA1.2"
            aXmlSerializer.AttributeL( KXmlXmlns, KPresenceSubListAttributesNS_CSP12 );
            }
        }
    else
        {
        // xmlns="Name space"
        // append attribute's namespace according to attributes on the list
        MPEngPresenceAttrManager* attributeManager =
            PEngAttrLibFactory::AttributeManagerInstanceLC(
                aSessionSlotId );
        // Append Attribute name space
        TPtrC8 attributeName;
        TPtrC8 attributeNameSpace;
        User::LeaveIfError( attributeManager->GetAttributeXmlNameAndNameSpace(
                                aAttributes[0],
                                attributeName,
                                attributeNameSpace ) );

        aXmlSerializer.AttributeL( KXmlXmlns, attributeNameSpace );

        // Append attributes one by one
        for ( TInt x( 0 ) ; x < count ; x++ )
            {
            User::LeaveIfError( attributeManager->GetAttributeXmlNameAndNameSpace(
                                    aAttributes[x],
                                    attributeName,
                                    attributeNameSpace ) );

            aXmlSerializer.StartTagL( attributeName
                                    ).EndTagL( attributeName );
            }
        CleanupStack::PopAndDestroy(); // attributeManager
        }

    // </PresenceSubList>
    aXmlSerializer.EndTagL( KPresenceSubList );
    }


// -----------------------------------------------------------------------------
// NListLibTools::AppendContactListPropertiesXmlL()
// -----------------------------------------------------------------------------
//
void NListLibTools::AppendContactListPropertiesXmlL(
    MPEngXMLSerializer& aXmlSerializer,
    const CPEngContactListSettings& aSettings )
    {
    // <ContactListProperties>
    // <Property>
    aXmlSerializer.StartTagL( KContactListProperties ).
    StartTagL( KProperty );

    // <Name>
    // DisplayName
    // </Name>
    aXmlSerializer.StartTagL( KName ).
    NarrowTextL( KDispName ).
    EndTagL( KName );

    // <Value>
    // display name value
    // </Value>
    aXmlSerializer.StartTagL( KValue ).
    UnicodeTextL( aSettings.DispName() ).
    EndTagL( KValue );
    // </Property>
    aXmlSerializer.EndTagL( KProperty );

    // <Property>
    aXmlSerializer.StartTagL( KProperty );

    // <Name>
    // Default
    // </Name>
    aXmlSerializer.StartTagL( KName ).
    NarrowTextL( KDefault ).
    EndTagL( KName );

    // <Value>
    // property value
    TInt newDefault( aSettings.Property( KPEngNewDefaultContactList,
                                         KPEngCntLstPropertyNativePermanent ) );

    TBool defaulList( ( aSettings.Property( KPEngCntLstDefault, KPEngCntLstPropertyNativePermanent ) &&
                        ( newDefault != KErrCancel ) ) ||
                      newDefault );
    // </Value>
    aXmlSerializer.StartTagL( KValue ).
    NarrowTextL( defaulList ?  KXMLValueTrue : KXMLValueFalse ).
    EndTagL( KValue );

    // </Property>
    // </ContactListProperties>
    aXmlSerializer.EndTagL( KProperty ).
    EndTagL( KContactListProperties );
    }


// -----------------------------------------------------------------------------
// NListLibTools::AppendContactListPropertiesXmlL()
// -----------------------------------------------------------------------------
//
void NListLibTools::ApppendReceiveListTagL(
    MPEngXMLSerializer& aXmlSerializer,
    TPEngWVCspVersion& aCSPVersion,
    TBool aValue )
    {
    // Only 1.2 version supports ReceiveList tag
    if ( aCSPVersion != EWVCspV12 )
        {
        return;
        }

    aXmlSerializer.StartTagL( KReceiveList ).
    NarrowTextL( aValue ? KXMLValueTrue : KXMLValueFalse ).
    EndTagL( KReceiveList );

    }
//  End of File

