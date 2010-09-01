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
*     Online methods.
*
*
*/


inline const CImpsKey* CImpsElement::GetKey( ) const
    {
    return iKey;
    }

inline CImpsKey* CImpsElement::GetKey( )
    {
    return iKey;
    }

inline TInt CImpsElement::GetInt( ) const
    {
    return iInt;
    }

inline TDesC* CImpsElement::GetString( ) const
    {
    return iString;
    }

inline TDesC8* CImpsElement::GetString8( ) const
    {
    return iString8;
    }

inline TBool CImpsElement::GetBoolean( ) const
    {
    return iBool;
    }

inline TBool CImpsElement::IsEmpty( ) const
    {
    return iIsEmpty;
    }

inline TImpsDataType CImpsElement::GetType(  ) const
    {
    return iValueType;
    }

inline void CImpsData::Reset( )
    {
    iElements->ResetAndDestroy( );
    }

inline TInt CImpsData::Count( ) const
    {
    return iElements->Count( );
    }

//-------------------------------------------------------
   

// End of File
