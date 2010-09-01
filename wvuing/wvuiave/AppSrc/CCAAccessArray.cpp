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
* Description:  Access list's array
 *
*/


// INCLUDE FILES
#include    "CCAAccessArray.h"
#include    "ChatDefinitions.h"
#include    "CAUtils.h"
#include    "IMUtils.h"
#include    "MCAGroupPropertiesPC.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAAccessArray::CCAAccessArray( MCAGroupPropertiesPC* aGroupPropsPC,
                                CDesCArray& aUserList )
        : iGroupPropertiesPC( aGroupPropsPC ),
        iAccessList( aUserList )
    {
    }

// Symbian OS default constructor can leave.
void CCAAccessArray::ConstructL()
    {
    iLine = HBufC::NewL( KMaxWVIDLength + KMaxTabLength );
    }

// Two-phased constructor.
CCAAccessArray* CCAAccessArray::NewL(
    MCAGroupPropertiesPC* aGroupPropertiesDialogPC,
    CDesCArray& aUserList )
    {
    CCAAccessArray* self = new ( ELeave ) CCAAccessArray(
        aGroupPropertiesDialogPC, aUserList );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCAAccessArray::~CCAAccessArray()
    {
    delete iLine;
    }

// ---------------------------------------------------------
// CCAAccessArray::AppendL( const TDesC16& aPtr )
// This method appends descriptor to the accesslist array
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessArray::AppendL( const TDesC& aPtr )
    {
    iAccessList.AppendL( aPtr );
    }

// ---------------------------------------------------------
// CCAAccessArray::Delete( TInt aIndex,TInt aCount )
// This method deletes items from accesslist
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessArray::Delete( TInt aIndex, TInt aCount )
    {
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    __ASSERT_ALWAYS( aCount + aIndex <= iAccessList.MdcaCount() ,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    iAccessList.Delete( aIndex, aCount );
    }

// ---------------------------------------------------------
// CCAAccessArray::Reset()
// This method resets the access list
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAccessArray::Reset()
    {
    iAccessList.Reset();
    }

// ---------------------------------------------------------
// CCAAccessArray::MdcaCount() const
// This method returns number of items in array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAccessArray::MdcaCount() const
    {
    return iAccessList.MdcaCount();
    }

// ---------------------------------------------------------
// CCAAccessArray::MdcaPoint( TInt aIndex ) const
// This method returns descriptor for given index
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCAAccessArray::MdcaPoint( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    __ASSERT_ALWAYS( aIndex < iAccessList.MdcaCount() ,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    TPtrC id( iAccessList.MdcaPoint( aIndex ) );

    TPtrC idPtr( KNullDesC );
    TRAP_IGNORE( idPtr.Set( iGroupPropertiesPC->GetIdentificationL( id ) ) );

    iLine->Des().Copy( idPtr.Left( iLine->Des().MaxLength() ) );

    TPtr linePtr( iLine->Des() );
    linePtr.Insert( 0, KTabulator );
    return iLine->Des();
    }

// ---------------------------------------------------------
// CCAAccessArray::RawData( TInt aIndex ) const
// This method returns unformatted descriptor from array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCAAccessArray::RawData( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    __ASSERT_ALWAYS( aIndex < iAccessList.MdcaCount() ,
                     User::Panic( KPanicText, EAccessArrayOutOfBounds ) );

    return iAccessList.MdcaPoint( aIndex );
    }

//  End of File
