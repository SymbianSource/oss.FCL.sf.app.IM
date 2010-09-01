/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contact sorter
*
*/


// INCLUDE FILES
#include    "CCAContactSorter.h"
#include    "MCAStoredContact.h"
#include    "TStorageManagerGlobals.h"
#include    "CCAStorageDefs.h"
#include 	"ChatDebugPrint.h"

// The order of contacts if sorted by status
const TInt KOnline  = 1;
const TInt KBusy	= 2;
const TInt KAway    = 3;
const TInt KOffline = 4;
const TInt KBlocked = 5;
const TInt KUnknown = 6;

// local functions
// sorting algorithms, you can add new one to extend functionality


// Compares alphabetically using MCAStoredContact::Identification and
// TDesC::CompareC
TInt CompareAlphabetically( const MCAStoredContact& aFirst,
                            const MCAStoredContact& aSecond )
    {
    return aFirst.Identification().CompareC( aSecond.Identification(),
                                             KCollationLevel, NULL );
    }


// Group the contact by it's status.
TInt OrderContact( const MCAStoredContact& aContact )
    {
    if ( aContact.IsBlocked() )
        {
        return KBlocked;   // Fourth group, contact is blocked
        }
    else
        {
        // Contact is not blocked, group by presence status
        switch ( aContact.OnlineStatus() )
            {
            case TStorageManagerGlobals::EOnline:
                {
                return KOnline;  // First group, contact is online
                }
            case TStorageManagerGlobals::EBusy:
                {
                return KBusy;  // First group, contact is online
                }
            case TStorageManagerGlobals::EAway:
                {
                return KAway;    // Second group, contact is away
                }
            case TStorageManagerGlobals::EOffline:
                {
                return KOffline; // Third group, contact is offline
                }
            default:
                {
                return KUnknown; // Fifth group, unknown status
                }
            }
        }
    }

// Compares by contact status, -1 = aFirst is greater, 1 = aSecond is greater.
TInt CompareByPresence( const MCAStoredContact& aFirst,
                        const MCAStoredContact& aSecond )
    {
    TInt firstGroup( OrderContact( aFirst ) );
    TInt secondGroup( OrderContact( aSecond ) );

    // If both contacts belong to same group, sort alphabetically
    if ( firstGroup == secondGroup )
        {
        return CompareAlphabetically( aFirst, aSecond );
        }

    // Otherwise return the difference of groups
    return firstGroup - secondGroup;
    }

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCAContactSorter* CCAContactSorter::NewL()
    {
    CCAContactSorter* self = new( ELeave ) CCAContactSorter();
    return self;
    }

// Destructor
CCAContactSorter::~CCAContactSorter()
    {
    }

// ---------------------------------------------------------
// CCAContactSorter::Sort( RPointerArray<MCAStoredContact>& aList )
// Sorts given array.
// ---------------------------------------------------------
//
void CCAContactSorter::Sort( RPointerArray< MCAStoredContact >& aList )
    {
    CHAT_DP_TXT( "CCAContactSorter::Sort - Enter" );
    aList.Sort( iAlgorithm );
    CHAT_DP_TXT( "CCAContactSorter::Sort - Done" );
    }

// ---------------------------------------------------------
// CCAContactSorter::Compare( const MCAStoredContact& aFirst,
//                                  const MCAStoredContact& aSecond ) const
// Get function pointer from inside of TLinearOrder and
// execute the function.
// ---------------------------------------------------------
//
TInt CCAContactSorter::Compare( const MCAStoredContact& aFirst,
                                const MCAStoredContact& aSecond ) const
    {
    return iAlgorithm.operator TGeneralLinearOrder()( &aFirst, &aSecond );
    }

// C++ default constructor can NOT contain any code, that
// might leave.
// Default algorithm is alphabetic. It is changed later in ConstructL.
//
CCAContactSorter::CCAContactSorter() :
        iAlgorithm( *CompareAlphabetically )
    {
    }

// ---------------------------------------------------------
// CCAContactSorter::SetSortAlgorithm()
// Get setting item value and choose algorithm.
// New algorithms can be added to this method.
// ---------------------------------------------------------
//
void CCAContactSorter::SetSortAlgorithm(
    MCAStoredContact::TSortAlgorithm aAlgorithm )
    {
    switch ( aAlgorithm )
        {
        case MCAStoredContact::ECompareByPresence:
            {
            iAlgorithm =
                TLinearOrder< MCAStoredContact > ( *CompareByPresence );
            break;
            }

        case MCAStoredContact::ECompareAlphabetically: // flowthrough
        default :
            {
            iAlgorithm =
                TLinearOrder< MCAStoredContact > ( *CompareAlphabetically );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCAContactSorter::InsertOrder()
// Get setting item value and choose algorithm.
// New algorithms can be added to this method.
// ---------------------------------------------------------
//
TLinearOrder< MCAStoredContact >& CCAContactSorter::InsertOrder()
    {
    return iAlgorithm;
    }

// End of file
