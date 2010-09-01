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
* Description: Contact DB reader class
*
*/



// INCLUDE FILES
#include "cntdbreader.h"
#include <cntdb.h>
#include <cntfield.h>
#include <cntitem.h>
#include <cntfldst.h>

// CONSTANTS
static const TUid EPbkFieldIdWVID = { KIntContactFieldVCardMapWV };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactDBReader::CContactDBReader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactDBReader::CContactDBReader()
    {

    }

// -----------------------------------------------------------------------------
// CContactDBReader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactDBReader::ConstructL()
    {
    iContactDB = CContactDatabase::OpenL();
    }

// -----------------------------------------------------------------------------
// CContactDBReader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactDBReader* CContactDBReader::NewL()
    {
    CContactDBReader* self = new( ELeave ) CContactDBReader();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CContactDBReader::~CContactDBReader()
    {
    delete iContactDB; // should this be Closed before deleting?
    }


// -----------------------------------------------------------------------------
// CContactDBReader::ReadWVIdsL
//
// -----------------------------------------------------------------------------
//
void CContactDBReader::ReadWVIdsL( const CContactIdArray& aContactArray,
                                   CDesCArray& aWVUserIds )
    {

    TInt count ( aContactArray.Count() );
    for ( TInt i( 0 ) ; i < count ; i++ )
        {
        HBufC* userId = NULL;
        CContactItem* newContacItem =
            iContactDB->ReadMinimalContactLC( aContactArray[i] ); // <<

        CContactItemFieldSet& cardFieldSet = newContacItem->CardFields();

        TInt fieldIndex ( cardFieldSet.FindNext(
                              EPbkFieldIdWVID,
                              KContactFieldSetSearchAll ) );

        // now read all wv ID fields
        while ( fieldIndex != KErrNotFound )
            {
            // TextStorage() does not transfer the ownership
            CContactTextField* textField =
                ( cardFieldSet[fieldIndex] ).TextStorage();

            userId = textField->Text( ).AllocL();
            CleanupStack::PushL( userId );
            aWVUserIds.AppendL( *userId );
            CleanupStack::Pop(); // >> userId
            fieldIndex = cardFieldSet.FindNext(
                             EPbkFieldIdWVID,
                             fieldIndex + 1 );
            }
        CleanupStack::PopAndDestroy( );// >> newContacItem
        }
    }

// -----------------------------------------------------------------------------
// CContactDBReader::ReadWVIdL
//
// -----------------------------------------------------------------------------
//
void CContactDBReader::GetWVIdL( const TContactItemId& aConatctId,
                                 CDesCArray& aWVUserIds )
    {
    HBufC* userId = NULL;
    CContactItem* newContacItem =
        iContactDB->ReadMinimalContactLC( aConatctId ); // <<

    CContactItemFieldSet& cardFieldSet = newContacItem->CardFields();

    TInt fieldIndex ( cardFieldSet.FindNext(
                          EPbkFieldIdWVID,
                          KContactFieldSetSearchAll ) );

    // now read all wv ID fields
    while ( fieldIndex != KErrNotFound )
        {
        // TextStorage() does not transfer the ownership
        CContactTextField* textField =
            ( cardFieldSet[fieldIndex] ).TextStorage();

        userId = textField->Text( ).AllocL();
        CleanupStack::PushL( userId );
        aWVUserIds.AppendL( *userId );
        CleanupStack::Pop(); // >> userId
        fieldIndex = cardFieldSet.FindNext(
                         EPbkFieldIdWVID,
                         fieldIndex + 1 );
        }
    CleanupStack::PopAndDestroy( );// >> newContacItem

    }

//  End of File
