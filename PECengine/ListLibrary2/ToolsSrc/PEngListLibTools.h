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

#ifndef PENGLISTLIBTOOLS_H__
#define PENGLISTLIBTOOLS_H__

//  INCLUDES
#include <e32base.h>
#include "PEngContactIdsTools.h"
#include "PEngWVCspVersion.h"
#include "CPEngContactListModBase.h"
#include "CPEngContactListModItemContainer.h"




// FORWARD DECLARATION
class CPEngSessionSlotId;
class MPEngXMLSerializer;
class MPEngXMLParser;
class CPEngContactListModBase;
class CPEngContactListModItemContainer;
class CPEngContactListSettings;
class CPEngTransactionStatus;
class MPEngContactListTransactionManager;





// CLASS DEFINITION


/**
 * Contaclist view wrapper.
 * Maps contact items contact ID to MDesCArray.
 */
NONSHARABLE_CLASS( TPEngContactlistDesViewWraper ) :
        public MDesCArray
    {
public:

    /**
     * C++ constructor.
     */
    inline TPEngContactlistDesViewWraper(
        CPEngContactListModBase& aContactList )
            : iContactList( aContactList )
        {
        }


public: // from MDesC16Array


    /**
     * Gets count of the wrapped array.
     * @return The count of array elements.
     */
    inline TInt MdcaCount() const
        {
        return iContactList.Count();
        }


    /**
     * Gets the element from the wrapped array.
     * @param aIndex The element index.
     * @return The array element.
     */
    inline TPtrC16 MdcaPoint( TInt aIndex ) const
        {
        return iContactList.ContactItem( aIndex ).Id();
        }

private: // data

    //OWN: The wrapped contact list
    CPEngContactListModBase&    iContactList;
    };




















// NAMESPACE DEFINITION

/**
 * Namespace encapsulating a collection of
 * List Library utility functions.
 */
namespace NListLibTools
    {
    /**
     * Leave if there is error except, KErrAlreadyExists.
     *
     * @since 3.0
     * @param aError The errorcode which to handle
     */
    void LeaveIfMajorErrorL( TInt aError );


    /**
     * Parses contacts in the nick list response.
     *
     * @since 3.0
     * @param aResponse buffer with response to parse
     * @param aParsedContacts array to store parsed contacts
     * @param aContactList contact list model to look for contacts
     * @param aXMLparser1 XML parser1
     * @param aXMLparser2 XML parser2
     * @param aUpdateNickname
     * @param aNewContactCount count is goint to be increased for each new contact
     */
    TBool ParseContactListNickListL(
        const TDesC8& aResponse,
        RPointerArray<CPEngContactListModItemContainer>& aParsedContacts,
        CPEngContactListModBase& aContactList,
        MPEngXMLParser& aXMLparser1,
        MPEngXMLParser& aXMLparser2,
        TBool aUpdateNickname,
        TInt* aNewContactCount = NULL );



    /**
     * Parses plain contact IDs from the list response.
     *
     * @since 3.0
     * @param aResponse buffer with response to parse
     * @param aParsedContacts array to store parsed contacts
     * @param aContactList contact list model to look for contacts
     * @param aXMLparser1 XML parser1
     * @param aXMLparser2 XML parser2
     * @param aUpdateNickname
     * @param aNewContactCount count is goint to be increased for each new contact
     */
    TBool ParseContactListIDsL(
        const TDesC8& aResponse,
        RPointerArray<CPEngContactListModItemContainer>& aParsedContacts,
        CPEngContactListModBase& aContactList,
        MPEngXMLParser& aXMLparser1,
        TBool aUpdateNickname,
        TInt* aNewContactCount = NULL );



    /**
     * Parse Contact list properties
     *
     * @since 3.0
     * @param aResponse buffer with response to parse
     * @param aSettings contact list settings, to store parsed data
     * @param aXMLparser1 XML parser1
     * @param aXMLparser2 XML parser2
     */
    TBool ParseContactListPropertiesL(
        const TDesC8& aResponse,
        CPEngContactListSettings& aSettings,
        MPEngXMLParser& aXMLparser1,
        MPEngXMLParser& aXMLparser2 );


    /**
     * Remove bad contact from the contact list(local, network view)
     *
     * @since 3.0
     * @param aTransactionStatus transaction status with the bad contact ids
     * @param aContactList contact list
     */
    void RemoveBadContactsL(
        const CPEngTransactionStatus& aTransactionStatus,
        CPEngContactListModBase& aContactList );


    /**
     * Remove bad contact Ids from the local view, so they will be removed
     * from the server at next transaction.
     *
     * @since 3.0
     * @param aTransactionStatus transaction status with the bad contact ids
     * @param aContactList contact list
     */
    void MarkForRemoveBadContactsL(
        const CPEngTransactionStatus& aTransactionStatus,
        CPEngContactListModBase& aContactList );



    /**
     * Harmonizes default contact list settings
     *
     * Function is called after properties of the contact list
     * have been updated. Harmonizes the default contact list settings
     *
     * @since 3.0
     * @param aCntListSettings The contact list.
     * @param aManager The contact list manager.
     */
    void HarmonizeDefaultContactListSettingsL(
        CPEngContactListSettings& aCntListSettings,
        MPEngContactListTransactionManager& aManager );




    /**
     * Register Presence attributes of the contacts from passed array
     * to be accepter by the presence engine
     *
     * @since 3.0
     * @param aContacts array of contact ids
     * @param aSessionSlotId Session slot Id
     */
    void ActivatePresenceAttributesL(
        const MDesCArray& aContacts,
        const CPEngSessionSlotId& aSessionSlotId );


    /**
     * Unregister Presence attributes of the contacts from passed array
     *
     * @since 3.0
     * @param aContacts array of contact ids
     * @param aSessionSlotId Session slot Id
     */
    void DeactivatePresenceAttributesL(
        const MDesCArray& aContacts,
        const CPEngSessionSlotId& aSessionSlotId );



    /**
     * Append transaction content to the start of the buffer
     * @since 3.0
     * @param aXMLSerializer XML serializer
     * @param aCSPVersion CPS Version
     */
    void AppendTransactionContentTagXmlL(
        MPEngXMLSerializer& aXMLSerializer,
        TPEngWVCspVersion& aCSPVersion );


    /**
     * Append contact list name to the XML buffer
     * @since 3.0
     * @param aXMLSerializer XML serializer
     * @param aSettings contact list settings
     */
    void AppendContactListNameXmlL(
        MPEngXMLSerializer& aXmlSerializer,
        const CPEngContactListSettings& aSettings );


    /**
     * Append Presence Attributes for subscription to the XML buffer
     *
     * @since 3.0
     * @param aAttributes
     * @param aXMLSerializer XML serializer
     * @param aSessionSlotId Session slot Id
     * @param aCSPVersion CSP Version
     */
    void AppendAttributesForSubscriptionXmlL(
        const RArray<TUint32> aAttributes,
        MPEngXMLSerializer& aXmlSerializer,
        const CPEngSessionSlotId& aSessionSlotId,
        TPEngWVCspVersion& aCSPVersion );



    /**
     * Append Contact list properties into the XML buffer
     *
     * @since 3.0
     * @param aXMLSerializer XML serializer
     * @param aSettings contact list settings
     */
    void AppendContactListPropertiesXmlL(
        MPEngXMLSerializer& aXmlSerializer,
        const CPEngContactListSettings& aSettings );



    /**
     * Append Contact list "ReceiveList" tag for the
     * "ListManage-Request" request, if it is supported
     *
     * @since 3.0
     * @param aXMLSerializer XML serializer
     * @param aCSPVersion version of the CSP
     * @param aValue value of the ReceiveList tag
     */
    void ApppendReceiveListTagL(
        MPEngXMLSerializer& aXmlSerializer,
        TPEngWVCspVersion& aCSPVersion,
        TBool aValue );

    } //namespace NListLibTools




#endif // PENGLISTLIBTOOLS_H__


//  End of File

