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
* imps fields class.
*
*/


// INCLUDE FILES
#include    <s32strm.h>     // streams 
#include    <e32def.h>
#include    "impsconst.h"
#include    "impsfields.h"
#include    "impsdatautils.h"
#include    "impsvariant.h"

// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsFields::CImpsFields()
    {
    }

// Symbian OS default constructor can leave.
void CImpsFields::ConstructL()
    {
    iData = CImpsData::NewL( );
    Reset( );
    }

// Two-phased constructor.
EXPORT_C CImpsFields* CImpsFields::NewL()
    {
    CImpsFields* self = new ( ELeave ) CImpsFields;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
EXPORT_C CImpsFields::~CImpsFields()
    {
    // notice: Do remember to update this list when you change the type of members!
    Reset();
    delete iData;
    }

// ---------------------------------------------------------
// CImpsFields::SetSessionIdL
// ---------------------------------------------------------
//
EXPORT_C void CImpsFields::SetSessionIdL( 
    const TDesC& aId )
    {
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TImpsDataUtils::SetSessionIDL( myAc, aId );
    CleanupStack::PopAndDestroy(1);     // >>> myAc
    }

// ---------------------------------------------------------
// CImpsFields::SessionIdL
// ---------------------------------------------------------
//
EXPORT_C TPtrC CImpsFields::SessionIdL()
    {
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TPtrC id;
    TImpsDataUtils::GetSessionIDL( myAc, id );
    CleanupStack::PopAndDestroy(1);     // >>> myAc
    return id;
    }


// ---------------------------------------------------------
// CImpsFields::SetSessionTypeL
// ---------------------------------------------------------
//
EXPORT_C void CImpsFields::SetSessionTypeL( TImpsSessionType aType )
    {
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TImpsDataUtils::SetSessionTypeL( myAc, aType );
    CleanupStack::PopAndDestroy(1);     // >>> myAc
    }

// ---------------------------------------------------------
// CImpsFields::SessionTypeL
// ---------------------------------------------------------
//
EXPORT_C TImpsSessionType CImpsFields::SessionTypeL()
    {
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TImpsSessionType id;
    TImpsDataUtils::GetSessionTypeL( myAc, id );
    CleanupStack::PopAndDestroy(1);     // >>> myAc
    return id;
    }

// ---------------------------------------------------------
// CImpsFields::SetTidL
// ---------------------------------------------------------
//
EXPORT_C void CImpsFields::SetTidL( 
    const TDesC& aId )
    {
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TImpsDataUtils::SetTransactionIDL( myAc, aId );
    CleanupStack::PopAndDestroy(1);     // >>> myAc
    }

// ---------------------------------------------------------
// CImpsFields::TidL
// ---------------------------------------------------------
//
EXPORT_C TPtrC CImpsFields::TidL()
    {
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TPtrC id;
    TImpsDataUtils::GetTransactionIDL( myAc, id );
    CleanupStack::PopAndDestroy(1);     // >>> myAc
    return id;
    }

// ---------------------------------------------------------
// CImpsFields::Reset
// ---------------------------------------------------------
//
EXPORT_C void CImpsFields::Reset( )
    {
    iMsgType = 0;
    
    iStatus = 0;

    if ( iData )
        {
        iData->Reset( );
        }
        
    iCspVersion = EImpsCspVersion11;
    }

// ---------------------------------------------------------
// CImpsFields::Size
// ---------------------------------------------------------
//
EXPORT_C TInt CImpsFields::Size() const
    {

    TInt sizeEntry = Align4( KImpsPackedCopyData );

    // notice: update this always when class defrinition changes
    TInt sizeStrings = 0;
    TInt size =  sizeEntry + sizeStrings + ( KImpsPackedFieldsNbr * sizeof(TInt32) );

    size += iData->Size( ) + (2 * sizeof (TInt) );   //size,count
    return size;
    }

#ifdef _DEBUG

// ---------------------------------------------------------
// CImpsFields::SetDataL
// ---------------------------------------------------------
//
void CImpsFields::SetDataL( CImpsData& aData)
    {
    delete iData;
    iData = NULL;
    iData = &aData;
    }
#endif

// ---------------------------------------------------------
// CImpsFields::ResponseStatusL
// ---------------------------------------------------------
//
EXPORT_C TInt32 CImpsFields::ResponseStatusL()
    {
    TInt code(0);
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TBool ret = TImpsDataUtils::GetResultStatusL( myKey, myAc, code );
    CleanupStack::PopAndDestroy(2);     // >>> myKey, myAc

#ifdef _FAKE_RESPONSE
    if( !ret )
        return KImpsStatusOk;
    else 
        return code;
#endif
    if( ret )
        return code;
    return 0;
    }

// ---------------------------------------------------------
// CImpsFields::PollL
// ---------------------------------------------------------
//
EXPORT_C TBool CImpsFields::PollL()
    {
    CImpsKey* myKey = CImpsKey::NewLC();    // <<< myKey
    CImpsDataAccessor* myAc = CImpsDataAccessor::NewLC( this );

    TBool polling(EFalse);
    TBool ret = TImpsDataUtils::GetPollL( myKey, myAc, polling );
    CleanupStack::PopAndDestroy(2);     // >>> myKey, myAc
    if( ret )
        return polling;
    return ret;
    }


#ifdef _DEBUG    
// ---------------------------------------------------------
// CImpsFields::DumpToFile
// ---------------------------------------------------------
//

void CImpsFields::DumpToFileL(RFs& aFs, const TDesC& aFilename)
    {
    iData->DumpToFileL(aFs, aFilename);
    }
#endif    

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  


