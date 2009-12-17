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
* Description:  Attribute model constuctor.
*
*/

// INCLUDE FILES
#include "CPEngPresenceAttrConstructor.h"
#include "CPEngAttrConstructorCon.h"
#include "MPEngPresenceAttrConstructorTypeImp.h"

#include "CPEngPresenceAttrModel.h"
#include "MPEngPresenceAttrModelTypeImp.h"


#include <E32Std.h>


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrConstructor* CPEngPresenceAttrConstructor::NewL(
    CPEngAttrConstructorCon& aContainer,
    MPEngPresenceAttrConstructorTypeImp& aTypeConstructor,
    TUint32 aType,
    TPEngAttributeOriginator aOrigin )
    {
    CPEngPresenceAttrConstructor* self = new ( ELeave ) CPEngPresenceAttrConstructor(
        aContainer,
        aTypeConstructor,
        aType,
        aOrigin );
    return self;
    }



// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngPresenceAttrConstructor::CPEngPresenceAttrConstructor(
    CPEngAttrConstructorCon& aContainer,
    MPEngPresenceAttrConstructorTypeImp& aTypeConstructor,
    TUint32 aType,
    TPEngAttributeOriginator aOrigin )
        : iType( aType ),
        iContainer( aContainer ),
        iTypeConstructor( &aTypeConstructor ),
        iOrigin( aOrigin )
    {
    }


// Destructor
CPEngPresenceAttrConstructor::~CPEngPresenceAttrConstructor()
    {
    delete iTypeConstructor;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::NewAttributeInstanceLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAdvancedAttrModel2* CPEngPresenceAttrConstructor::NewAttributeInstanceLC(
    TBool aUserModel,
    const TDesC& aPresenceID )
    {
    MPEngPresenceAttrModelTypeImp* typeImp = iTypeConstructor->NewAttributeInstanceLC( aUserModel );
    CPEngPresenceAttrModel* model = CPEngPresenceAttrModel::NewL( *this,
                                                                  aPresenceID,
                                                                  typeImp );
    CleanupStack::Pop(); //typeImp
    CleanupStack::PushL( model );

    return model;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::OpenRef()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrConstructor::OpenRef()
    {
    iContainer.OpenRef();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::CloseRef()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrConstructor::CloseRef()
    {
    iContainer.CloseRef();
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::GetCurrentXmlNames()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrConstructor::GetCurrentXmlNames( TPtrC8& aAttributeName,
                                                       TPtrC8& aAttributeNameSpace ) const
    {
    return iTypeConstructor->GetCurrentCspName( aAttributeName, aAttributeNameSpace );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::Type()
// -----------------------------------------------------------------------------
//
TUint32 CPEngPresenceAttrConstructor::Type() const
    {
    return iType;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::Originator()
// -----------------------------------------------------------------------------
//
TPEngAttributeOriginator CPEngPresenceAttrConstructor::Originator() const
    {
    return iOrigin;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrConstructor::GetProperty( TUint aPropertyName,
                                                TUint aPropertyLevel,
                                                TInt& aProperty ) const
    {
    return iTypeConstructor->GetProperty( aPropertyName, aPropertyLevel, aProperty );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrConstructor::GetProperty( TUint aPropertyName,
                                                TUint aPropertyLevel,
                                                TDes8& aProperty ) const
    {
    return iTypeConstructor->GetProperty( aPropertyName, aPropertyLevel, aProperty );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::GetProperty()
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrConstructor::GetPropertyPtr( TUint aPropertyName,
                                                   TUint aPropertyLevel,
                                                   TPtrC16& aProperty ) const
    {
    return iTypeConstructor->GetPropertyPtr( aPropertyName, aPropertyLevel, aProperty );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::Extension()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrTypePropertiesExt2* CPEngPresenceAttrConstructor::Extension()
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::TypeExtension()
// -----------------------------------------------------------------------------
//
TAny* CPEngPresenceAttrConstructor::TypeExtension( TUint32 aType )
    {
    return iTypeConstructor->TypeExtension( aType );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrConstructor::__DbgPanicL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrConstructor::__DbgPanicL( TPEngAttributeConstructorPanics aReason,
                                                TInt aLeaveCode ) const

    {
#if defined(_DEBUG)
    User::Panic( KPEngAttributeConstructorPanicCategory, aReason );
#else
    User::Leave( aLeaveCode );
#endif

    // suppress the unreferenced variable warnings
    ( void ) aReason;
    ( void ) aLeaveCode;
    }


//  End of File

