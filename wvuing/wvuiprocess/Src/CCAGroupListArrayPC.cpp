/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Group List Array
 *
*/


// INCLUDE FILES
#include    "ImpsGroupProps.h"
#include    "CCAGroupListArrayPC.h"
#include    "cautils.h"
#include    "CCAPCUtils.h"
#include    "MCAStoredGroup.h"
#include    "MCAStoredGroups.h"
#include    "MCAExtendedStoredGroup.h"
#include    "chatdefinitions.h"
#include	"ChatDebugPrint.h"
#include    <barsread.h>
#include    <eikenv.h>
#include    <collate.h>

// local functions
// comparison against whether the group is joined, or if they mismatch, then
// the group name
TInt Compare( const MCAStoredGroup& aFirst, const MCAStoredGroup& aSecond )
    {
    TBool firstJ( aFirst.IsJoined() );
    TBool secondJ( aSecond.IsJoined() );

    if ( firstJ && !secondJ )
        {
        // first group is joined, but second not..
        // so first group should be before second
        return -1; // any negative will do
        }
    else if ( !firstJ && secondJ )
        {
        // first group not joined, but second is..
        // so first group should be after second
        return 1; // any positive will do
        }
    else
        {
        // both groups joined or not..
        // so they should be compared equally

        // Create custom collation method to ignore punctuations
        // index 0 gets the default method
        TCollationMethod collation =
            *Mem::CollationMethodByIndex( 0 );
        collation.iFlags |= TCollationMethod::EIgnoreNone;

        return aFirst.GroupName().CompareC( aSecond.GroupName(),
                                            KCollationLevel, &collation );
        }
    }




// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAGroupListArrayPC::CCAGroupListArrayPC()
    {
    }

// Symbian OS default constructor can leave.
void CCAGroupListArrayPC::ConstructL()
    {
    iLine = HBufC::NewL( KMaxChatGroupLenght + KMaxTabLength );
    }

// Two-phased constructor.
CCAGroupListArrayPC* CCAGroupListArrayPC::NewL()
    {
    CCAGroupListArrayPC* self = new ( ELeave ) CCAGroupListArrayPC();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCAGroupListArrayPC::~CCAGroupListArrayPC()
    {
    iGroupArray.Close();
    delete iLine;
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::MdcaCount() const
// Returns number of items in array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupListArrayPC::MdcaCount() const
    {
    return iGroupArray.Count();
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::MdcaPoint()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCAGroupListArrayPC::MdcaPoint( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0, User::Panic( KPanicText,
                                               EChatListArrayIndexOutOfBounds ) );
    __ASSERT_ALWAYS( aIndex < iGroupArray.Count(),
                     User::Panic( KPanicText, EChatListArrayIndexOutOfBounds ) );

    TPtr data( iLine->Des() );
    if ( iGroupArray[aIndex]->GroupName().Length() > 0 )
        {
        // show group name if we have one
        data.Copy(
            iGroupArray[aIndex]->GroupName().Left(
                data.MaxLength() - KMaxTabLength ) );
        }
    else
        {
        // ...else show group id
        data.Copy( CCAPCUtils::DisplayId(
                       iGroupArray[aIndex]->GroupId().Left(
                           data.MaxLength() - KMaxTabLength ) ) );
        }


    CHAT_DP( D_CHAT_LIT( "CCAGroupListArrayPC::MdcaPoint [%d] %S" ), aIndex, &data );
    return iLine->Des();
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::operator[]
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAStoredGroup* CCAGroupListArrayPC::operator[] ( TInt aIndex ) const
    {
    return iGroupArray[ aIndex ];
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::Sort
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupListArrayPC::Sort()
    {
    // This method could be called even before any calls have been made to
    // CCAGroupListArrayPC::Add (e.g. after PopulateGroupsListL), so this method
    // cannot be removed.
    TLinearOrder<MCAStoredGroup> linOrder( *Compare );
    iGroupArray.Sort( linOrder );
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::Add
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupListArrayPC::Add( const MCAStoredGroup* aGroup )
    {
    // try to keep it in order anyway
    TLinearOrder<MCAStoredGroup> order( *Compare );
    iGroupArray.InsertInOrderAllowRepeats( aGroup, order );
    }


// ---------------------------------------------------------
// CCAGroupListArrayPC::Remove
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAGroupListArrayPC::Remove( TInt aIndex )
    {
    CHAT_DP( D_CHAT_LIT( "CCAGroupListArrayPC::Remove %d" ), aIndex );
    if ( ( aIndex >= 0 ) && ( aIndex < iGroupArray.Count() ) )
        {
        iGroupArray.Remove( aIndex );
        iGroupArray.Compress();
        }
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::Array
// (other items were commented in a header).
// ---------------------------------------------------------
//
RPointerArray<MCAStoredGroup>& CCAGroupListArrayPC::Array()
    {
    return iGroupArray;
    }

// ---------------------------------------------------------
// CCAGroupListArrayPC::Find
// Note, we can't use FindInOrder since we are just given the groupid
// and the array stores MCAStoredGroup, and the TDesC doesn't know
// into which group it belongs to.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAGroupListArrayPC::Find( const TDesC& aGroupId )
    {
    TInt count( iGroupArray.Count() );

    // Create custom collation method to ignore punctuations
    // index 0 gets the default method
    TCollationMethod collation =
        *Mem::CollationMethodByIndex( 0 );
    collation.iFlags |= TCollationMethod::EIgnoreNone;

    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iGroupArray[i]->GroupId().CompareC(
                 aGroupId, KCollationLevel, &collation ) == 0 )
            {
            return i;
            }
        }
    CHAT_DP( D_CHAT_LIT( "CCAGroupListArrayPC::Find::not found!" ) );
    return KErrNotFound;
    }


// ---------------------------------------------------------
// CCAGroupListArrayPC::GroupAt
// (other items were commented in a header).
// ---------------------------------------------------------
//
MCAStoredGroup* CCAGroupListArrayPC::GroupAt( TInt aIndex ) const
    {

    __ASSERT_ALWAYS( aIndex >= 0, User::Panic( KPanicText,
                                               EChatListArrayIndexOutOfBounds ) );
    __ASSERT_ALWAYS( aIndex < iGroupArray.Count(),
                     User::Panic( KPanicText, EChatListArrayIndexOutOfBounds ) );

    return iGroupArray[aIndex];
    }


//  End of File
