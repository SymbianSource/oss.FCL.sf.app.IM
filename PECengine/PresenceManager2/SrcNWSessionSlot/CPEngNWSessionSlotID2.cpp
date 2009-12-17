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
* Description:  NWSessionSlotID.
*
*/

// INCLUDE FILES
#include <CPEngNWSessionSlotID2.h>
#include "CPEngNWSessionSlotID2Imp.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotID2* CPEngNWSessionSlotID2::NewL()
    {
    CPEngNWSessionSlotID2* self = CPEngNWSessionSlotID2::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEngNWSessionSlotID2* CPEngNWSessionSlotID2::NewLC()
    {
    CPEngNWSessionSlotID2* self = new ( ELeave ) CPEngNWSessionSlotID2;
    CleanupStack::PushL( self );
    self->iImp = CPEngNWSessionSlotID2Imp::NewL();
    return self;
    }


EXPORT_C CPEngNWSessionSlotID2* CPEngNWSessionSlotID2::CloneL() const
    {
    CPEngNWSessionSlotID2* self = new ( ELeave ) CPEngNWSessionSlotID2;
    CleanupStack::PushL( self );
    self->iImp = iImp->CloneL();
    CleanupStack::Pop( self );
    return self;
    }



// Destructor
CPEngNWSessionSlotID2::~CPEngNWSessionSlotID2()
    {
    delete iImp;
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::CPEngNWSessionSlotID2
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2::CPEngNWSessionSlotID2()
    {
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetServiceAddressL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetServiceAddressL( const TDesC& aAddress )
    {
    iImp->SetServiceAddressL( aAddress, EPEngMMExact );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetUserIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetUserIdL( const TDesC& aUserId )
    {
    iImp->SetUserIdL( aUserId, EPEngMMExact );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetAppIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetAppIdL( const TDesC& aAppId )
    {
    iImp->SetAppIdL( aAppId, EPEngMMExact );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetAllL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetAllL( const TDesC& aAddress,
                                              const TDesC& aUserId,
                                              const TDesC& aAppId )
    {
    iImp->SetServiceAddressL( aAddress, EPEngMMExact );
    iImp->SetUserIdL( aUserId, EPEngMMExact );
    iImp->SetAppIdL( aAppId, EPEngMMExact );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::ServiceAddress()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngNWSessionSlotID2::ServiceAddress() const
    {
    return iImp->ServiceAddress();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::UserId()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngNWSessionSlotID2::UserId() const
    {
    return iImp->UserId();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::AppId()
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CPEngNWSessionSlotID2::AppId() const
    {
    return iImp->AppId();
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetServiceAddressMatchAnyL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetServiceAddressMatchAnyL()
    {
    iImp->SetServiceAddressL( KPEngMatchSymbolWildAny, EPEngMMWildAny );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetUserIdMatchAnyL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetUserIdMatchAnyL()
    {
    iImp->SetUserIdL( KPEngMatchSymbolWildAny, EPEngMMWildAny );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::SetAppIdMatchAnyL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::SetAppIdMatchAnyL()
    {
    iImp->SetAppIdL( KPEngMatchSymbolWildAny, EPEngMMWildAny );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::IsServiceAddressWild()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlotID2::IsServiceAddressWild() const
    {
    return iImp->IsServiceAddressWild();
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::IsUserIdWild()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlotID2::IsUserIdWild() const
    {
    return iImp->IsUserIdWild();
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::IsAppIdWild()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlotID2::IsAppIdWild() const
    {
    return iImp->IsAppIdWild();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::IsWild()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPEngNWSessionSlotID2::IsWild() const
    {
    return iImp->IsWild();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::MatchFullId()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotID2::MatchFullId(
    const CPEngNWSessionSlotID2& aIdToMatch ) const
    {
    return iImp->MatchFullId( *aIdToMatch.Implementation() );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::MatchBasePart()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotID2::MatchBasePart(
    const CPEngNWSessionSlotID2& aIdToMatch ) const
    {
    return iImp->MatchBasePart( *aIdToMatch.Implementation() );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::MatchAppIdPart()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEngNWSessionSlotID2::MatchAppIdPart(
    const CPEngNWSessionSlotID2& aIdToMatch ) const
    {
    return iImp->MatchAppIdPart( *aIdToMatch.Implementation() );
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::PackL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngNWSessionSlotID2::PackL() const
    {
    HBufC8* pack = iImp->PackFullLC();
    CleanupStack::Pop( pack );
    return pack;
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::PackLC()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CPEngNWSessionSlotID2::PackLC() const
    {
    return iImp->PackFullLC();
    }


// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::UnPackL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEngNWSessionSlotID2::UnPackL( const TDesC8& aPack )
    {
    iImp->UnPackFullL( aPack );
    }



// -----------------------------------------------------------------------------
// CPEngNWSessionSlotID2::Implementation()
// -----------------------------------------------------------------------------
//
CPEngNWSessionSlotID2Imp* CPEngNWSessionSlotID2::Implementation()
    {
    return iImp;
    }

const CPEngNWSessionSlotID2Imp* CPEngNWSessionSlotID2::Implementation() const
    {
    return iImp;
    }


// End of File


