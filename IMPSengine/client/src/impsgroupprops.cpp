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
* imps group property class.
*
*/


// INCLUDE FILES
#include    "ImpsGroupProps.h"
#include    "ImpsGroupPropsInt.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CImpsGroupProp::CImpsGroupProp
// ---------------------------------------------------------
//
CImpsGroupProp::CImpsGroupProp()
    {
    }

// ---------------------------------------------------------
// CImpsGroupProp::~CImpsGroupProp
// ---------------------------------------------------------
//
CImpsGroupProp::~CImpsGroupProp()
    {
    delete iKey;
    delete iValue; 
    iLink.Deque();   
    }

// ---------------------------------------------------------
// CImpsGroupProp::NewL
// ---------------------------------------------------------
//
CImpsGroupProp* CImpsGroupProp::NewL(
                const TDesC& aKey, 
                const TDesC& aValue )
    {
    CImpsGroupProp* self = new (ELeave) CImpsGroupProp;
    CleanupStack::PushL( self );
    self->ConstructL( aKey, aValue );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsGroupProp::Key
// ---------------------------------------------------------
//
TPtrC CImpsGroupProp::Key() const
    {
/*LDRA_NOANALYSIS*/
    return iKey ? TPtrC( *iKey ) : TPtrC();
/*LDRA_ANALYSIS*/
    }

// ---------------------------------------------------------
// CImpsGroupProp::Value
// ---------------------------------------------------------
//
TPtrC CImpsGroupProp::Value() const
    {
/*LDRA_NOANALYSIS*/
    return iValue ? TPtrC( *iValue ) : TPtrC();
/*LDRA_ANALYSIS*/
    }

// ---------------------------------------------------------
// CImpsGroupProp::ConstructL
// ---------------------------------------------------------
//
void CImpsGroupProp::ConstructL(
                const TDesC& aKey, 
                const TDesC& aValue )
    {
    HBufC* newAttrib = aKey.AllocL();
    iKey = newAttrib;
    newAttrib = aValue.AllocL();
    iValue = newAttrib;
    }

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CImpsGroupProps::CImpsGroupProps()
:   iReset( ETrue ),
    iPropertyList(_FOFF( CImpsGroupProp, iLink )),
    iIter( iPropertyList ),
    iPrivateMessages( EImpsPropUndef )
    {
    }

// ---------------------------------------------------------
// CImpsGroupProps::~CImpsGroupProps()
// ---------------------------------------------------------
//
CImpsGroupProps::~CImpsGroupProps()
    {
    // Delete all properties
    TDblQueIter<CImpsGroupProp> rIter( iPropertyList );

    rIter.SetToFirst();

    while ( rIter )
        {    
        CImpsGroupProp* pr = rIter;
        rIter++;
        delete pr;
        }  
    }

// ---------------------------------------------------------
// CImpsGroupProps::ConstructL
// ---------------------------------------------------------
//
void CImpsGroupProps::ConstructL()
    {
    }    

// ---------------------------------------------------------
// CImpsGroupProps::Count
// ---------------------------------------------------------
//
EXPORT_C TInt CImpsGroupProps::Count() 
    {
    // Count all properties
    TInt counter = 0;
    TDblQueIter<CImpsGroupProp> rIter( iPropertyList );
    rIter.SetToFirst();
    while ( rIter )
        {    
        rIter++;
        counter++;
        } 
    return counter;
    }
    
// ---------------------------------------------------------
// CImpsGroupProps::AddL
// ---------------------------------------------------------
//
EXPORT_C void CImpsGroupProps::AddL( const CImpsGroupProp& aProperty )
    {
    iReset =  EFalse;
    CImpsGroupProp* pr = CImpsGroupProp::NewL(
            aProperty.iKey->Des(), 
            aProperty.iValue->Des() );
    iPropertyList.AddLast( *pr );
    }

// ---------------------------------------------------------
// CImpsGroupProps::FirstL
// ---------------------------------------------------------
//
EXPORT_C const CImpsGroupProp& CImpsGroupProps::FirstL()
    {
    iReset = EFalse;
    iIter.SetToFirst();
    CImpsGroupProp* pr = iIter;
    if ( !pr )
        {
        User::Leave( KErrEof );
        }
    return *pr;
    }

// ---------------------------------------------------------
// CImpsGroupProps::NextL
// ---------------------------------------------------------
//
EXPORT_C const CImpsGroupProp& CImpsGroupProps::NextL()
    {
    if ( iReset )
        {
        User::Leave( KErrEof );
        }
    iIter++;
    CImpsGroupProp* pr = iIter;
    if ( !pr )
        {
        User::Leave( KErrEof );
        }
    return *pr;
    }

// ---------------------------------------------------------
// CImpsGroupProps::Reset
// ---------------------------------------------------------
//
void CImpsGroupProps::Reset()
    {
    iReset = ETrue;
    // Delete all properties
    TDblQueIter<CImpsGroupProp> rIter( iPropertyList );

    rIter.SetToFirst();

    while ( rIter )
        {    
        CImpsGroupProp* pr = rIter;
        rIter++;
        delete pr;
        } 

    iPrivateMessages = EImpsPropUndef;
    }

// ---------------------------------------------------------
// CImpsGroupProps::IsPrivateAllowed
// ---------------------------------------------------------
//
TImpsPropertyBool CImpsGroupProps::IsPrivateAllowed() const
    {
    return iPrivateMessages;
    }

// ---------------------------------------------------------
// CImpsGroupProps::IsPrivateAllowed
// ---------------------------------------------------------
//
void CImpsGroupProps::SetPrivateAllowed( TImpsPropertyBool aValue )
    {
    iPrivateMessages = aValue;
    }


/************ PRIVATE PROPS ********************************/

// ---------------------------------------------------------
// CImpsCommonGroupProps::CImpsCommonGroupProps
// ---------------------------------------------------------
//
CImpsCommonGroupProps::CImpsCommonGroupProps()
: CImpsGroupProps(),
  iAccessOpen( EImpsPropUndef), 
  iTypePublic( EImpsPropUndef ), 
  iSearchable( EImpsPropUndef ),  
  iNbrActive(0),  
  iMaxUsers(0),   
  iName(NULL),        
  iTopic(NULL),
  iWelcome(NULL),
  iAutoDelete( EImpsPropUndef )      
    {

    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::NewL
// ---------------------------------------------------------
//
EXPORT_C CImpsCommonGroupProps* CImpsCommonGroupProps::NewL()
    {
    CImpsCommonGroupProps* self = new (ELeave) CImpsCommonGroupProps;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::~CImpsCommonGroupProps
// ---------------------------------------------------------
//
CImpsCommonGroupProps::~CImpsCommonGroupProps()
    {
    Reset();
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::Reset
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::Reset()
    {
    iAccessOpen = EImpsPropUndef;
    // iTypePublic = EImpsPropUndef;
    iSearchable = EImpsPropUndef;
    // iNbrActive = EImpsPropUndef;
    iMaxUsers = EImpsPropUndef;
    delete iName;
    iName = NULL;
    delete iTopic;
    iTopic = NULL;
    delete iWelcome;
    iWelcome = NULL;
    CImpsGroupProps::Reset();
    }


// ---------------------------------------------------------
// CImpsCommonGroupProps::Type
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyType CImpsCommonGroupProps::Type() const
    {
    return EImpsPropCommon;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::IsPrivateAllowed
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyBool CImpsCommonGroupProps::IsPrivateAllowed() const
    {
    return CImpsGroupProps::IsPrivateAllowed();
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetPrivateAllowed
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetPrivateAllowed( 
    TImpsPropertyBool aValue )
    {
    CImpsGroupProps::SetPrivateAllowed( aValue );
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::IsOpen
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyBool CImpsCommonGroupProps::IsOpen() const
    {
    return iAccessOpen;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetOpen
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetOpen( 
    TImpsPropertyBool aValue )
    {
    iAccessOpen = aValue;
    }


// ---------------------------------------------------------
// CImpsCommonGroupProps::IsPublic
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyBool CImpsCommonGroupProps::IsPublic() const
    {
    return iTypePublic;
    }


// ---------------------------------------------------------
// CImpsCommonGroupProps::IsSearchable
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyBool CImpsCommonGroupProps::IsSearchable() const
    {
    return iSearchable;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetSearchable
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetSearchable( 
    TImpsPropertyBool aValue )
    {
    iSearchable = aValue;
    }


// ---------------------------------------------------------
// CImpsCommonGroupProps::NbrOfUsers
// ---------------------------------------------------------
//
EXPORT_C TInt CImpsCommonGroupProps::NbrOfUsers() const
    {
    return iNbrActive;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::MaxNbrOfUsers
// ---------------------------------------------------------
//
EXPORT_C TInt CImpsCommonGroupProps::MaxNbrOfUsers() const
    {
    return iMaxUsers;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetMaxNbrOfUsers
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetMaxNbrOfUsers( 
    TInt aValue )
    {
    iMaxUsers = aValue;
    }


// ---------------------------------------------------------
// CImpsCommonGroupProps::GroupName
// ---------------------------------------------------------
//
EXPORT_C TPtrC CImpsCommonGroupProps::GroupName() const
    {
/*LDRA_NOANALYSIS*/
    return iName ? TPtrC( *iName ) : TPtrC();
/*LDRA_ANALYSIS*/
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetGroupNameL
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetGroupNameL( 
    const TDesC& aName )
    {
    HBufC* newAttrib = aName.AllocL();
    delete iName;
    iName = newAttrib;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::Topic
// ---------------------------------------------------------
//
EXPORT_C TPtrC CImpsCommonGroupProps::Topic() const
    {
/*LDRA_NOANALYSIS*/
    return iTopic ? TPtrC( *iTopic ) : TPtrC();
/*LDRA_ANALYSIS*/
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetTopicL
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetTopicL( 
    const TDesC& aTopic )
    {
    HBufC* newAttrib = aTopic.AllocL();
    delete iTopic;
    iTopic = newAttrib;
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::Welcome
// ---------------------------------------------------------
//
EXPORT_C TPtrC CImpsCommonGroupProps::Welcome() const
    {
/*LDRA_NOANALYSIS*/
    return iWelcome ? TPtrC( *iWelcome ) : TPtrC();
/*LDRA_ANALYSIS*/
    }

// ---------------------------------------------------------
// CImpsCommonGroupProps::SetWelcomeL
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetWelcomeL( 
    const TDesC& aText )
    {
    HBufC* newAttrib = aText.AllocL();
    delete iWelcome;
    iWelcome = newAttrib;
    }
// ---------------------------------------------------------
// CImpsCommonGroupProps::AutoDelete
// ---------------------------------------------------------
//    
EXPORT_C TImpsPropertyBool CImpsCommonGroupProps::AutoDelete() const
	 {
	 return iAutoDelete;	
	 }
// ---------------------------------------------------------
// CImpsCommonGroupProps::SetAutoDelete
// ---------------------------------------------------------
//
EXPORT_C void CImpsCommonGroupProps::SetAutoDelete( TImpsPropertyBool aValue )
	{
	iAutoDelete = aValue;		
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


/************ PRIVATE PROPS ********************************/

// ---------------------------------------------------------
// CImpsPrivateGroupProps::CImpsPrivateGroupProps
// ---------------------------------------------------------
//
CImpsPrivateGroupProps::CImpsPrivateGroupProps()
: CImpsGroupProps(),
  iMember( EImpsPropUndef ),
  iPrivilege( EImpsUserUndef )
    {

    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::NewL
// ---------------------------------------------------------
//
EXPORT_C CImpsPrivateGroupProps* CImpsPrivateGroupProps::NewL()
    {
    CImpsPrivateGroupProps* self = new (ELeave) CImpsPrivateGroupProps;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::~CImpsPrivateGroupProps
// ---------------------------------------------------------
//
CImpsPrivateGroupProps::~CImpsPrivateGroupProps()
    {
    
    }

// Symbian OS default constructor can leave.
void CImpsPrivateGroupProps::ConstructL()
    {
    Reset( );
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::Reset
// ---------------------------------------------------------
//
EXPORT_C void CImpsPrivateGroupProps::Reset()
    {
    iMember = EImpsPropUndef;
    iPrivilege = EImpsUserUndef;
    CImpsGroupProps::Reset();    
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::Type
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyType CImpsPrivateGroupProps::Type() const
    {
    return EImpsPropPrivate;    
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::IsPrivateAllowed
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyBool CImpsPrivateGroupProps::IsPrivateAllowed() const
    {
    return CImpsGroupProps::IsPrivateAllowed();
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::SetPrivateAllowed
// ---------------------------------------------------------
//
EXPORT_C void CImpsPrivateGroupProps::SetPrivateAllowed( TImpsPropertyBool aValue )
    {
    CImpsGroupProps::SetPrivateAllowed( aValue );
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::IsMember
// ---------------------------------------------------------
//
EXPORT_C TImpsPropertyBool CImpsPrivateGroupProps::IsMember() const
    {
    return iMember;
    }

// ---------------------------------------------------------
// CImpsPrivateGroupPropsInt::HasData
// ---------------------------------------------------------
//
TBool CImpsPrivateGroupPropsInt::HasData() const
    {
    if ( ( iMember != EImpsPropUndef ) ||
         ( iPrivilege != EImpsUserUndef ) ||
         ( iPrivateMessages != EImpsPropUndef ) )  // from base class
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CImpsPrivateGroupProps::Privileges
// ---------------------------------------------------------
//
EXPORT_C TImpsGroupUsers CImpsPrivateGroupProps::Privileges() const
    {
    return iPrivilege;
    }


// EPOC default constructor can leave.
void CImpsCommonGroupProps::ConstructL()
    {
    Reset( );
    }

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
// CImpsCommonGroupPropsInt::Size
// ---------------------------------------------------------
//
TInt CImpsCommonGroupPropsInt::Size() const
    {
    TInt sizeEntry = Align4( KImpsPackedCommonProps );
    sizeEntry += Align4( KImpsPackedProps );

    TInt sizeStrings = 0;
    sizeStrings += GroupName().Size();
    sizeStrings += Topic().Size();
    sizeStrings += Welcome().Size();
    
    TInt size =  sizeEntry + sizeStrings + ( KImpsPackedCommonPropsNbr * sizeof(TInt32) );

    // no arrays

    return size;
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

