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
* Description:  Templated attribute data helpers.
*
*/

#ifndef __PENGWVATTRIBUTEMODELDATAHELPER_H
#define __PENGWVATTRIBUTEMODELDATAHELPER_H

//  INCLUDES
#include <E32Base.h>
#include <S32Strm.h>


/**
 * Templated attribute data helper.
 *
 *
 * @since 3.0
 */
template< class TBufType, class TDataType >
inline void DoSetDescL( TBufType*& aPtr, const TDataType& aData );



/**
 * Templated attribute data helper.
 *
 *
 * @since 3.0
 */
template< class TBufType, class TPtrType >
inline void DoGetDesc( const TBufType* const& aBuffer, TPtrType& aData );


/**
 * Templated attribute data helper.
 *
 *
 * @since 3.0
 */
template< class TBufType >
inline TInt DoGetBufExtSize( const TBufType* const& aBuffer );



/**
 * Templated attribute data helper.
 *
 *
 * @since 3.0
 */
template< class TBufType >
inline void DoExtrBufL( const TBufType* const& aBuffer, RWriteStream& aStream );



/**
 * Templated attribute data helper.
 *
 *
 * @since 3.0
 */
template< class TBufType, class TPtrType >
inline void DoIntrBufL( TBufType*& aBufPtr, TPtrType aPtr, RReadStream& aStream );


//Inline implementation
#include "PEngWVAttributeModelDataHelper.inl"


#endif      //__PENGWVATTRIBUTEMODELDATAHELPER_H

//  End of File

