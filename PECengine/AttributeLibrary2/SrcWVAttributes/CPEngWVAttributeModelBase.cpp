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
* Description:  Base class for WV attribute models.
*
*/

// INCLUDE FILES
#include <E32Base.h>
#include "CPEngWVAttributeModelBase.h"
#include "PEngWVAttributeModelBasePanics.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::CPEngWVAttributeModelBase
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVAttributeModelBase::CPEngWVAttributeModelBase( TBool aUserOwnAttribute )
        : iUserOwnModel( aUserOwnAttribute )
    {
    }


// Destructor
CPEngWVAttributeModelBase::~CPEngWVAttributeModelBase()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::SetDataL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVAttributeModelBase::SetDataL( TInt* /*aIntData*/,
                                           const TDesC8* /*a8Data*/,
                                           const TDesC16* /*a16Data*/,
                                           TInt /*aField*/,
                                           TInt /*aGroup*/ )
    {
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::SetDataAsyncL()
// -----------------------------------------------------------------------------
//
TBool CPEngWVAttributeModelBase::SetDataAsyncL( const TDesC8* /*a8Data*/,
                                                const TDesC16* /*a16Data*/,
                                                TRequestStatus& /*aRequestStatus*/,
                                                TInt /*aField*/,
                                                TInt /*aGroup*/ )
    {
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::CancelSetDataAsync()
// -----------------------------------------------------------------------------
//
void CPEngWVAttributeModelBase::CancelSetDataAsync()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::TypeExtension()
// -----------------------------------------------------------------------------
//
TAny* CPEngWVAttributeModelBase::TypeExtension( TUint32 /*aType*/ )
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::HandleNetworkSessionStateChangeL()
// -----------------------------------------------------------------------------
//
void CPEngWVAttributeModelBase::HandleNetworkSessionStateChangeL( TBool /*aSessionOpen*/ )
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::__AssertAttrEnumValueOutOfBoundsL()
// -----------------------------------------------------------------------------
//
void CPEngWVAttributeModelBase::__AssertAttrEnumValueOutOfBoundsL() const
    {
#if defined(_DEBUG)
    __AssertAttrEnumValueOutOfBounds();
#else
    User::Leave( KErrArgument );
#endif
    }

// -----------------------------------------------------------------------------
// CPEngWVAttributeModelBase::__AssertAttrEnumValueOutOfBounds()
// -----------------------------------------------------------------------------
//
void CPEngWVAttributeModelBase::__AssertAttrEnumValueOutOfBounds() const
    {
#if defined(_DEBUG)
    User::Panic( KWVAttrModBasePanic, EAttrEnumValueOutOfBounds );
#endif
    }



//  End of File
