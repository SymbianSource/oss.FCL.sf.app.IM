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
* Description: Implements the CIMStatusFetcher and the MImStatusList interface.
*
*/



// INCLUDE FILES
#include "imstatusimpl.h"
#include "apiutils.h"
#include "impspresence.h"

// CONSTANTS
//_LIT( KUserAvailability, "UserAvailability" );
_LIT( KOnLineStatus, "OnlineStatus" );
_LIT( KCommCaps,     "CommCap" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMStatusListImpl::CIMStatusListImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMStatusListImpl::CIMStatusListImpl( CImpsPresenceList& aPresList )
        :   iPresList( aPresList ),
        iCurrentPresence( NULL ),
        iCurrentIndex( -1 )
    {
    }


// -----------------------------------------------------------------------------
// CIMStatusListImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMStatusListImpl::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIMStatusList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMStatusListImpl* CIMStatusListImpl::NewL( CImpsPresenceList& aPresList )
    {
    CIMStatusListImpl* self = new( ELeave ) CIMStatusListImpl( aPresList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }



// Destructor
CIMStatusListImpl::~CIMStatusListImpl()
    {
    }


// -----------------------------------------------------------------------------
// CIMStatusListImpl::Count
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMStatusListImpl::Count()
    {
    return iPresList.Count();
    }

// -----------------------------------------------------------------------------
// CIMStatusListImpl::IMStatusL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TIMAttribute CIMStatusListImpl::IMStatusL( TInt aIndex )
    {
    if ( iCurrentIndex != aIndex )
        {
        iCurrentPresence = &iPresList.PresenceL( aIndex );
        iCurrentIndex = aIndex;
        }

    // get the attribute
    TIMAttribute imAttr = EIMOffline;
    CImpsSubsList* subslist = iCurrentPresence->DefaultSubsListL( );
//    CImpsUserAvailability* ua = NULL;
    CImpsOnlineStatus* os = NULL;
    CImpsCommCap* cc = NULL;


    if ( subslist )
        {
        CImpsPresenceAttr* attr = subslist->FirstAttribute();
        while ( attr )
            {
            /*
                        if( !attr->Name( ).Compare( KUserAvailability ) )
                            ua = STATIC_CAST( CImpsUserAvailability*, attr );
            */
            if ( !attr->Name( ).Compare( KOnLineStatus ) )
                os = STATIC_CAST( CImpsOnlineStatus*, attr );
            if ( !attr->Name( ).Compare( KCommCaps ) )
                cc = STATIC_CAST( CImpsCommCap*, attr );
            attr = subslist->NextAttribute();
            }
        }

    if ( os->Value() == EImpsTrue && cc->Cap( EImpsCapIM ) == EImpsTrue )
        {
        imAttr = EIMOnline;
        }

    return imAttr;
    }
// -----------------------------------------------------------------------------
// CIMStatusListImpl::OwnerUserIdL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CIMStatusListImpl::OwnerUserIdL( TInt aIndex )
    {
    if ( iCurrentIndex != aIndex )
        {
        iCurrentPresence = &iPresList.PresenceL( aIndex );
        iCurrentIndex = aIndex;
        }

    TImpsPresOwner ownerType;
    // get the user
    return iCurrentPresence->Owner( ownerType );
    }


//  End of File
