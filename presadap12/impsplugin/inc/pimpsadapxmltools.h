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
* Description:  Various tools for XML
*
*/

#ifndef __NIMPSADAPXMLTOOLS_H__
#define __NIMPSADAPXMLTOOLS_H__

//  INCLUDES
#include <e32base.h>
#include "PEngXMLTags.h"
#include "impsplugintags.h"
#include "PImpsAdapWVCspVersion.h"



// FORWARD DECLARATION

class MPEngXMLSerializer;
class MPEngXMLParser;


// NAMESPACE DEFINITION

/**
 * Namespace encapsulating a collection of
 * List Library utility functions.
 */
namespace NImpsAdapXMLTools
    {
    /**
     * Leave if there is error except, KErrAlreadyExists.
     *
     * @since 3.0
     * @param aError The errorcode which to handle
     */
    void LeaveIfMajorErrorL( TInt aError );

#if 0
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
    /*
    TBool ParseContactListIDsL(
                       const TDesC8& aResponse,
                       RPointerArray<CPEngContactListModItemContainer>& aParsedContacts,
                       CPEngContactListModBase& aContactList,
                       MPEngXMLParser& aXMLparser1,
                       TBool aUpdateNickname,
                       TInt* aNewContactCount = NULL );


    */
    /**
     * Parse Contact list properties
     *
     * @since 3.0
     * @param aResponse buffer with response to parse
     * @param aSettings contact list settings, to store parsed data
     * @param aXMLparser1 XML parser1
     * @param aXMLparser2 XML parser2
     */
    /*
    TBool ParseContactListPropertiesL(
                       const TDesC8& aResponse,
                       CPEngContactListSettings& aSettings,
                       MPEngXMLParser& aXMLparser1,
                       MPEngXMLParser& aXMLparser2 );

    */
    /**
     * Remove bad contact from the contact list(local, network view)
     *
     * @since 3.0
     * @param aTransactionStatus transaction status with the bad contact ids
     * @param aContactList contact list
     */
    /*
    void RemoveBadContactsL(
                       const CPEngTransactionStatus& aTransactionStatus,
                       CPEngContactListModBase& aContactList );

    */
    /**
     * Remove bad contact Ids from the local view, so they will be removed
     * from the server at next transaction.
     *
     * @since 3.0
     * @param aTransactionStatus transaction status with the bad contact ids
     * @param aContactList contact list
     */
    /*
    void MarkForRemoveBadContactsL(
                       const CPEngTransactionStatus& aTransactionStatus,
                       CPEngContactListModBase& aContactList );

    */

#endif

    /**
    * Append transaction content to the start of the buffer
    * @since 3.0
    * @param aXMLSerializer XML serializer
    * @param aCSPVersion CPS Version
    */
    void AppendTransactionContentTagXmlL(
        MPEngXMLSerializer& aXMLSerializer,
        TImpsAdapWVCspVersion aCSPVersion );

    void AppendUserIdXmlL( MPEngXMLSerializer& aXmlSerializer,
                           const TDesC16& aUserId );

    /**
     * Append Contact list "ReceiveList" tag for the
     * "ListManage-Request" request, if it is supported
     *
     * @since 3.0
     * @param aXMLSerializer XML serializer
     * @param aCSPVersion version of the CSP
     * @param aValue value of the ReceiveList tag
     */
    void ApppendReceiveListTagL( MPEngXMLSerializer& aXmlSerializer,
                                 TImpsAdapWVCspVersion& aCSPVersion,
                                 TBool aValue );

    void AppendContactListNameXmlL( MPEngXMLSerializer& aXmlSerializer,
                                    const TDesC16& aContactListName );

#if 0


    /**
     * Append Presence Attributes for subscription to the XML buffer
     *
     * @since 3.0
     * @param aAttributes
     * @param aXMLSerializer XML serializer
     * @param aSessionSlotId Session slot Id
     * @param aCSPVersion CSP Version
     */
    //void AppendAttributesForSubscriptionXmlL(
    //                    const RArray<TUint32> aAttributes,
    //                    MPEngXMLSerializer& aXmlSerializer,
    //                    const CPEngSessionSlotId& aSessionSlotId,
    //                    TPEngWVCspVersion& aCSPVersion );



    /**
     * Append Contact list properties into the XML buffer
     *
     * @since 3.0
     * @param aXMLSerializer XML serializer
     * @param aSettings contact list settings
     */
    /*
    void AppendContactListPropertiesXmlL(
                       MPEngXMLSerializer& aXmlSerializer,
                       const CPEngContactListSettings& aSettings );
    */



#endif

    }




#endif // __NIMPSADAPXMLTOOLS_H__


//  End of File

