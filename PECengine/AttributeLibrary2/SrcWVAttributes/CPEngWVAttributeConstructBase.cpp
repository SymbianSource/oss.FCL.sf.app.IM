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
* Description:  Base class for WV attribute constructors.
*
*/

// INCLUDE FILES
#include <E32Base.h>
#include "CPEngWVAttributeConstructBase.h"



//LOCAL constants
namespace
    {
    //Panic
    _LIT( KWVAttrConstructBasePanic, "WVAttrConstBase" );

    //Panic reasons
    enum TWVAttrConstructBasePanicReasons
        {
        EWVCspNotSupported
        };

    void WVAttrConstructBasePanic( TWVAttrConstructBasePanicReasons aPanicReason )
        {
        User::Panic( KWVAttrConstructBasePanic, aPanicReason );
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::CPEngWVAttributeConstructBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngWVAttributeConstructBase::CPEngWVAttributeConstructBase(
    TPEngWVCspVersion aCurrentCspVer,
    const TWVCspAttributeNameEntry* aNameTable,
    TInt aTableCount )
        : iCurrentCspVer( aCurrentCspVer ),
        iCspNameTable( aNameTable ),
        iCspNameTableCount( aTableCount )
    {
    __ASSERT_ALWAYS( ProtocolNameTableIndex( aCurrentCspVer ) != KErrNotFound,
                     WVAttrConstructBasePanic( EWVCspNotSupported ) );
    }



// Destructor
CPEngWVAttributeConstructBase::~CPEngWVAttributeConstructBase()
    {
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngWVAttributeConstructBase::GetProperty( TUint /*aPropertyName*/,
                                                 TUint /*aPropertyLevel*/,
                                                 TInt& /*aProperty*/ ) const
    {
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngWVAttributeConstructBase::GetProperty( TUint /*aPropertyName*/,
                                                 TUint /*aPropertyLevel*/,
                                                 TDes8& /*aProperty*/ ) const
    {
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::GetPropertyPtr()
// -----------------------------------------------------------------------------
//
TInt CPEngWVAttributeConstructBase::GetPropertyPtr( TUint /*aPropertyName*/,
                                                    TUint /*aPropertyLevel*/,
                                                    TPtrC16& /*aProperty*/ ) const

    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::TypeExtension()
// -----------------------------------------------------------------------------
//
TAny* CPEngWVAttributeConstructBase::TypeExtension( TUint32 /*aType*/ )
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::GetCurrentCspName()
// -----------------------------------------------------------------------------
//
TInt CPEngWVAttributeConstructBase::GetCurrentCspName( TPtrC8& aAttributeName,
                                                       TPtrC8& aAttributeNameSpace ) const
    {
    TInt ix = ProtocolNameTableIndex( iCurrentCspVer );

    if ( ix != KErrNotFound )
        {
        const TWVCspAttributeNameEntry& entry = iCspNameTable[ ix ];
        aAttributeName.Set( *entry.iName );
        aAttributeNameSpace.Set( *entry.iNameSpace );
        if ( ( aAttributeName.Length() > 0 ) &&
             ( aAttributeNameSpace.Length() > 0 ) )
            {
            return KErrNone;
            }

        return KErrArgument;
        }

    return KErrNotSupported;
    }




// -----------------------------------------------------------------------------
// CPEngWVAttributeConstructBase::ProtocolNameTableIndex()
// -----------------------------------------------------------------------------
//
TInt CPEngWVAttributeConstructBase::ProtocolNameTableIndex(
    TPEngWVCspVersion aCspVer ) const
    {
    for ( TInt ix = 0; ix < iCspNameTableCount; ix++ )
        {
        const TWVCspAttributeNameEntry& entry = iCspNameTable[ ix ];
        if ( entry.iCspVersion == aCspVer )
            {
            return ix;
            }
        }

    return KErrNotFound;
    }


//  End of File
