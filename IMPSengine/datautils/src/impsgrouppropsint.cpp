/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* imps common group props class.
*
*/


// INCLUDE FILES
#include    "impsgroupprops.h"
#include    "impsgrouppropsint.h"


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::CImpsCommonGroupPropsInt
// ---------------------------------------------------------
//
CImpsCommonGroupPropsInt::CImpsCommonGroupPropsInt()
: CImpsCommonGroupProps()
    {

    }


// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::NewL
// ---------------------------------------------------------
//
EXPORT_C CImpsCommonGroupPropsInt* CImpsCommonGroupPropsInt::NewL()
    {
    CImpsCommonGroupPropsInt* self = new ( ELeave ) CImpsCommonGroupPropsInt;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::~CImpsCommonGroupPropsInt
// ---------------------------------------------------------
//
CImpsCommonGroupPropsInt::~CImpsCommonGroupPropsInt()
    {
    Reset();
    }

// Symbian OS default constructor can leave.
void CImpsCommonGroupPropsInt::ConstructL()
    {
    Reset( );
    }

// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::Reset
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupPropsInt::Reset()
    {
    CImpsCommonGroupProps::Reset();
    iTypePublic = EImpsPropUndef; // type public/private
    iNbrActive = EImpsPropUndef;   // nbr of active users
    }

// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::HasData
// ---------------------------------------------------------
//
TBool CImpsCommonGroupPropsInt::HasData() const
    {
    if ( ( iAccessOpen != EImpsPropUndef ) ||
         ( iTypePublic != EImpsPropUndef ) ||
         ( iSearchable != EImpsPropUndef ) ||
         ( iNbrActive >= 0  ) ||
         ( iMaxUsers >= 0  ) ||
         ( iName ) ||
         ( iTopic ) ||
         ( iWelcome ) ||
         ( iPrivateMessages != EImpsPropUndef ) )  // from base class
        {
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::SetPublic
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupPropsInt::SetPublic( 
    TImpsPropertyBool aType )
    {
    iTypePublic = aType;
    }

// ---------------------------------------------------------
// CImpsCommonGroupPropsInt::SetNbrOfUsers
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupPropsInt::SetNbrOfUsers( 
    TInt aNbr ) 
    {
    iNbrActive = aNbr;
    }


// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::CImpsPrivateGroupPropsInt
// ---------------------------------------------------------
//
CImpsPrivateGroupPropsInt::CImpsPrivateGroupPropsInt()
: CImpsPrivateGroupProps()
    {

    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::NewL
// ---------------------------------------------------------
//
EXPORT_C CImpsPrivateGroupPropsInt* CImpsPrivateGroupPropsInt::NewL()
    {
    CImpsPrivateGroupPropsInt* self = new (ELeave) CImpsPrivateGroupPropsInt;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::~CImpsPrivateGroupPropsInt
// ---------------------------------------------------------
//
CImpsPrivateGroupPropsInt::~CImpsPrivateGroupPropsInt()
    {
    Reset();
    }

// Symbian OS default constructor can leave.
void CImpsPrivateGroupPropsInt::ConstructL()
    {
    Reset( );
    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::Reset
// ---------------------------------------------------------
//
EXPORT_C void CImpsPrivateGroupPropsInt::Reset()
    {
    CImpsPrivateGroupProps::Reset();
    iMember = EImpsPropUndef;        // Am i member
    iPrivilege = EImpsUserUndef;     // privilege level
    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::SetMember
// ---------------------------------------------------------
//
EXPORT_C void CImpsPrivateGroupPropsInt::SetMember(
    TImpsPropertyBool aValue )
    {
    iMember = aValue;
    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::SetPrivileges
// ---------------------------------------------------------
//
EXPORT_C void CImpsPrivateGroupPropsInt::SetPrivileges(
    TImpsGroupUsers aValue  )
    {
    iPrivilege = aValue;
    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::Size
// ---------------------------------------------------------
//
TInt CImpsPrivateGroupPropsInt::Size() const
    {
	TInt sizeEntry = Align4( KImpsPackedPrivateProps );
    sizeEntry += Align4( KImpsPackedProps );

    // Notice: update this always when class definition changes
	TInt sizeStrings = 0;
	
    TInt size =  sizeEntry + sizeStrings;

    // no arrays

    return size;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  

