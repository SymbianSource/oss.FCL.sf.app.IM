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
* inline function assemble for data accessor.
*
*/

// INCLUDE FILES
#include    <e32std.h>
#include    "impsfields.h"
#include    "impsconst.h"
#include    "impskey.h"
#include    "impserrors.h"
#include    "impsutils.h"
#include    "impsdatautils.h"
#include    "impstdataaccessor.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CImpsDataAccessor::NewKeyL
// ---------------------------------------------------------
//
inline MImpsKey* CImpsDataAccessor::NewKeyL()
    {
    return iDataAccessor.NewKeyL();
    }

// ---------------------------------------------------------
// CImpsDataAccessor::StoreDescL
// mutator for symbian descriptor value
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::StoreDescL( MImpsKey* aKey, TPtrC aValue )
    {
    iDataAccessor.StoreDescL( aKey, aValue );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::StoreDescL8
// mutator for symbian 8-bit descriptor value
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::StoreDesc8L( MImpsKey* aKey, TPtrC8 aValue)
    {
    iDataAccessor.StoreDesc8L( aKey, aValue);
    }

// ---------------------------------------------------------
// CImpsDataAccessor::StoreInteger
// mutator for symbian integer value
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::StoreIntegerL( MImpsKey* aKey, TInt aValue )
    {
    iDataAccessor.StoreIntegerL( aKey, aValue );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::StoreBooleanL
// mutator for symbian boolean value
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::StoreBooleanL( MImpsKey* aKey, TBool aValue )
    {
    iDataAccessor.StoreBooleanL( aKey, aValue );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::StoreEmptyL
// mutator for symbian empty value
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::StoreEmptyL( MImpsKey* aKey)
    {
    iDataAccessor.StoreEmptyL(aKey);
    }

// ---------------------------------------------------------
// CImpsDataAccessor::StoreExtL
// mutator for extension attribute
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::StoreExtL( MImpsKey* aKey, 
                                          TPtrC8 aName, TPtrC8 aValue)
    {
    iDataAccessor.StoreExtL(aKey, aName, aValue);
    }

//----------------------------------------------------------
// CImpsDataAccessor::RestoreDescL
// accessor for symbian descriptor value
// ---------------------------------------------------------
//

inline TBool CImpsDataAccessor::RestoreDescL( MImpsKey* aKey, TDesC*& aDes)
    {
    return iDataAccessor.RestoreDescL( aKey, aDes);
    }

//----------------------------------------------------------
// CImpsDataAccessor::RestoreDesc8L
// accessor for symbian 8-bit descriptor value
// ---------------------------------------------------------
//

inline TBool CImpsDataAccessor::RestoreDesc8L( MImpsKey* aKey, TDesC8*& aDes )
    {
    return iDataAccessor.RestoreDesc8L( aKey, aDes );
    }

//----------------------------------------------------------
// CImpsDataAccessor::RestoreIntegerL
// accessor for integer value
// (covers also boolean and enum types
// ---------------------------------------------------------
//

inline TBool CImpsDataAccessor::RestoreIntegerL( MImpsKey* aKey, TInt& aInt )
    {
    return iDataAccessor.RestoreIntegerL( aKey, aInt );
    }

//----------------------------------------------------------
// CImpsDataAccessor::RestoreBooleanL
// accessor for integer value
// (covers also boolean and enum types
// ---------------------------------------------------------
//

inline TBool CImpsDataAccessor::RestoreBooleanL( MImpsKey* aKey, TBool& aBool )
    {
    return iDataAccessor.RestoreBooleanL( aKey, aBool );
    }

//----------------------------------------------------------
// CImpsDataAccessor::RestoreEmptyL
// accessor for integer value
// (covers also boolean and enum types
// ---------------------------------------------------------
//
inline TBool CImpsDataAccessor::RestoreEmptyL( MImpsKey* aKey )
    {
    return iDataAccessor.RestoreEmptyL( aKey );
    }

//----------------------------------------------------------
// CImpsDataAccessor::RestoreExtL
// accessor for extension attribute
// ---------------------------------------------------------
//
inline TBool CImpsDataAccessor::RestoreExtL( MImpsKey* aKey,  
                                             TDesC8*& aName, TDesC8*& aValue)
    {
    return iDataAccessor.RestoreExtL( aKey, aName, aValue );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::KeyTypeL
// method returns type of related to given key
// ---------------------------------------------------------
//
inline TImpsDataType CImpsDataAccessor::KeyTypeL( MImpsKey* aKey )
    {
    return iDataAccessor.KeyTypeL( aKey );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::RestoreAlternativeL
// ---------------------------------------------------------
//
inline TBool CImpsDataAccessor::RestoreAlternativeL( const MImpsKey* aKey, 
                                                TInt& aEnum, 
                                                TInt& aIndex, 
                                                TImpsKeyType& aType ) const
    {
    return iDataAccessor.RestoreAlternativeL( aKey, aEnum, aIndex, aType );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::CheckBranchExistenceL
// ---------------------------------------------------------
//
inline TBool CImpsDataAccessor::CheckBranchExistenceL( 
                                                const MImpsKey* aKey ) const
    {
    return iDataAccessor.CheckBranchExistenceL( aKey );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::NbrOfTransactionsL
// ---------------------------------------------------------
//
inline TInt CImpsDataAccessor::NbrOfTransactionsL( ) const
    {
    return iDataAccessor.NbrOfTransactionsL( );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::GetTransactionL
// ---------------------------------------------------------
//
inline TBool CImpsDataAccessor::GetTransactionL( 
    TInt aIndex, CImpsFields* aFields ) const
    {
    return iDataAccessor.GetTransactionL( aIndex, aFields );
    }

// ---------------------------------------------------------
// CImpsDataAccessor::GetImpsFields
// ---------------------------------------------------------
//
inline CImpsFields* CImpsDataAccessor::GetImpsFields() const
    {
    return iDataAccessor.GetImpsFields();
    }
    
#ifdef _DEBUG
// ---------------------------------------------------------
// CImpsDataAccessor::DumpToFileL
// ---------------------------------------------------------
//
inline void CImpsDataAccessor::DumpToFileL(RFs& aFs, const TDesC& aFilename)
	{
	iDataAccessor.DumpToFileL(aFs, aFilename);
	}
#endif	
//  End of File  
