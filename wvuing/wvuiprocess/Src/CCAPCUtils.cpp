/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utils for IM modules.
*
*/


#include "CCAPCUtils.h"
#include "CCAStorageManagerFactory.h"
#include "MCAStoredContacts.h"
#include "ChatDebugPrint.h"
#include "impsbuilddefinitions.h"

#include <EscapeUtils.h>
#include <collate.h>
#include "MCAError.h"
#include "CCAProcessManagerFactory.h"
#include "MCAProcessManager.h"
#include "CCAEngine.h"
#include "MCAStoredContact.h"
#include "CAUtils.h"

// general colon needed in various places
_LIT( KColon, ":" );

// general slash needed in various places
_LIT( KSlash, "/" );

// needed to skip domains in domain-neutral comparison
_LIT( KAt, "@" );

_LIT( KSpace, " " );


// -----------------------------------------------------------------------------
// CCAPCUtils::DisplayId
// -----------------------------------------------------------------------------
//


EXPORT_C TPtrC CCAPCUtils::DisplayId( const TDesC& aId, TBool aListHiding )
    {
    TPtrC ret( aId );

    MCAStoredContacts* contacts = NULL;
    TRAPD( err, contacts = CCAStorageManagerFactory::ContactListInterfaceL() );
    if ( err != KErrNone || !contacts )
        {
        // some error, don't modify
        CHAT_DP_FUNC_DP( "DisplayId", "Got some error, not hiding" );
        return ret;
        }

    // if we've branded the feature out, don't modify anything
    if ( contacts->WVHiding() || contacts->WVHidingPrefixOnly() )
        {
        // locate ":" for userid, groupid.
        // locate "/" for list id.
        TInt pos = aId.FindC( aListHiding ? KSlash : KColon );

        if ( ( pos != KErrNotFound ) && ( pos != aId.Length() - 1 ) )
            {
            // contains the special character, and it is not the last char
            // remove everything before the special char (including the char)
            ret.Set( aId.Mid( pos + 1 ) );
            }
        if ( contacts->WVHiding() || aListHiding )
            {
            // remove also the domain part
            TInt domainPos = ret.FindC( KAt );
            if ( ( domainPos != KErrNotFound ) && ( domainPos != 0 ) )
                {
                ret.Set( ret.Mid( 0, domainPos ) );
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CCAPCUtils::CreateBufLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCAPCUtils::CreateBufLC( const CDesCArray& aArray, TBool aDisplayId /*= EFalse*/ )
    {
    // calculate required space for the combined buffer
    TInt bufLength( 0 );
    TInt count( aArray.MdcaCount() );
    TInt i( 0 );
    for ( ; i < count; ++i )
        {
        bufLength += aArray.MdcaPoint( i ).Length() + KSpace().Length();
        }

    // allocate the buffer and fill it up
    HBufC* buf = HBufC::NewLC( bufLength );
    TPtr bufPtr( buf->Des() );
    for ( i = 0; i < count; ++i )
        {
        TPtrC ptr( aArray.MdcaPoint( i ) );
        if ( aDisplayId )
            {
            ptr.Set( DisplayId( ptr ) );
            }

        bufPtr.Append( ptr );
        bufPtr.Append( KSpace );
        }
    return buf;
    }
// -----------------------------------------------------------------------------
// CCAPCUtils::PartialSucessCountL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CCAPCUtils::PartialSucessCountL()
    {
    MCAProcessManager *processManager =  CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine* engine = processManager->GetEngine();
    const RPointerArray<MCAError>* errorArray = engine->PartialSuccessData();
    return errorArray->Count();
    }
// -----------------------------------------------------------------------------
// CCAPCUtils::GetErrorCodeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CCAPCUtils::GetErrorCodeL( TInt aIndex )
    {
    MCAProcessManager *processManager =  CCAProcessManagerFactory::InstanceL()->GetProcessManager();
    CCAEngine* engine = processManager->GetEngine();
    const RPointerArray<MCAError>* errorArray = engine->PartialSuccessData();
    return ( *errorArray )[ aIndex ]->Code();
    }

// -----------------------------------------------------------------------------
// CCAPCUtils::RemoveDuplicateWVIds
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCAPCUtils::RemoveDuplicateWVIds( CDesCArray& aArray )
    {
    TInt count = aArray.Count();

    // Go through whole array
    for ( TInt i = 0; i < count; i++ )
        {
        const TDesC& curId = aArray[i];

        for ( TInt j = i + 1; j < count; j++ )
            {
            if ( CAUtils::NeutralCompare( curId, aArray[j] ) == 0 )
                {
                // Duplicate found, remove from array
                aArray.Delete( j );
                count = aArray.Count();
                j--; // decrease by one to keep index the same in next for-loop
                }
            }
        }
    }
