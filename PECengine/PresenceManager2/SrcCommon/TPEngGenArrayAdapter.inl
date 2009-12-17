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
* Description:  One value adapter for generic array.
*
*/

// INCLUDE FILES

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
template < class T >
inline TPEngGenArrayAdapter< T >::TPEngGenArrayAdapter( const T& aValue )
    : iValue( aValue )
    {
    }


//Destructor
template < class T >
inline TPEngGenArrayAdapter< T >::~TPEngGenArrayAdapter()
    {
    }


// -----------------------------------------------------------------------------
// TPEngGenArrayAdapter::Array()
// -----------------------------------------------------------------------------
//
template < class T >
inline TArray< T > TPEngGenArrayAdapter< T >::Array() const
    {
    return TArray< T >( CountFunction,
                        AtFunction,
                        REINTERPRET_CAST( const CBase*,this) );
    }


// -----------------------------------------------------------------------------
// TPEngGenArrayAdapter::CountFunction()
// -----------------------------------------------------------------------------
//
template < class T >
inline TInt TPEngGenArrayAdapter< T >::CountFunction( const CBase* /*aThis*/ )
    {
    return 1;
    }


// -----------------------------------------------------------------------------
// TPEngGenArrayAdapter::AtFunction()
// -----------------------------------------------------------------------------
//
template < class T >
inline const TAny* TPEngGenArrayAdapter< T >::AtFunction( const CBase* aThis,
                                                          TInt aIndex )
    {
    //there is just one adapted value
    __ASSERT_ALWAYS( aIndex == 0, User::Panic( _L("GenArrAdpt"), KErrNotFound ) );

    const TPEngGenArrayAdapter* self = reinterpret_cast< const TPEngGenArrayAdapter* >( aThis );
    return &self->iValue;
    }


//  End of File

