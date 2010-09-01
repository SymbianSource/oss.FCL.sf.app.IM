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


//  INCLUDES
#include "PImpsAdapXMLTools.h"


#include "PEngXMLTags.h"
#include "MPEngXMLParser.h"
#include "MPEngXMLSerializer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NImpsAdapXMLTools::LeaveIfMajorErrorL()
// -----------------------------------------------------------------------------
//
void NImpsAdapXMLTools::LeaveIfMajorErrorL( TInt aError )
    {
    if ( ( aError < 0 ) &&
         ( aError != KErrAlreadyExists ) )
        {
        User::Leave( aError );
        }
    }


// -----------------------------------------------------------------------------
// NImpsAdapXMLTools::ParseContactListNickListL()
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// NImpsAdapXMLTools::AppendTransactionContentTagXmlL()
// -----------------------------------------------------------------------------
//
void NImpsAdapXMLTools::AppendTransactionContentTagXmlL(
    MPEngXMLSerializer& aXMLSerializer,
    TImpsAdapWVCspVersion aCSPVersion )
    {
    if ( aCSPVersion == EWVCspV11 )
        {
        aXMLSerializer.StartTagL(  KTransactionContent );
        aXMLSerializer.AttributeL( KXmlXmlns, KTransactionContentNS );
        }
    else if ( aCSPVersion == EWVCspV12 )
        {
        aXMLSerializer.StartTagL( KTransactionContent );
        aXMLSerializer.AttributeL( KXmlXmlns, KTransactionContentNS_CSP12 );
        }
    // add for EWVCspV13
    }

// -----------------------------------------------------------------------------
// NImpsAdapXMLTools::AppendUserIdXmlL()
// -----------------------------------------------------------------------------
//
void NImpsAdapXMLTools::AppendUserIdXmlL(
    MPEngXMLSerializer& aXmlSerializer,
    const TDesC16& aUserId )
    {
    // <UserID>
    // Contact list name value
    // </UserID>
    aXmlSerializer.StartTagL( KUserIDXMLTag
                            ).NarrowTextL( KWVXMLTag
                                         ).WvAddressL( aUserId
                                                     ).EndTagL( KUserIDXMLTag );
    }



// -----------------------------------------------------------------------------
// NImpsAdapXMLTools::ApppendReceiveListTagL()
// -----------------------------------------------------------------------------
//
void NImpsAdapXMLTools::ApppendReceiveListTagL(
    MPEngXMLSerializer& aXmlSerializer,
    TImpsAdapWVCspVersion& aCSPVersion,
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


// -----------------------------------------------------------------------------
// NImpsAdapXMLTools::AppendContactListNameXmlL()
// -----------------------------------------------------------------------------
//
void NImpsAdapXMLTools::AppendContactListNameXmlL( MPEngXMLSerializer& aXmlSerializer,
                                                   const TDesC16& aContactListName )
    {
    // <ContactList>
    // Contact list name value
    // </ContactList>
    aXmlSerializer.StartTagL( KContactList
                            ).WvAddressL( aContactListName
                                        ).EndTagL( KContactList );
    }




//  End of File

