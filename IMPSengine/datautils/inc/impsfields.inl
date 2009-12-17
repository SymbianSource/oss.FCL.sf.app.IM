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
*     Internal data structure online methods.
*
*
*/


inline void CImpsFields::SetMessageType( TInt aType )
   {
   iMsgType = aType;
   }

inline TInt CImpsFields::MessageType() const
   {
   return iMsgType;
   }

inline void CImpsFields::SetStatus( const TInt aStatus )
    {
    iStatus = aStatus;
    }


inline TInt CImpsFields::Status() const
    {
    return iStatus;
    }

inline CImpsData* CImpsFields::Data() const
    {
    return iData;
    }

inline void CImpsFields::SetCspVersion( TImpsCspVersion aCspVersion )
    {
    iCspVersion = aCspVersion;
    }

inline TImpsCspVersion CImpsFields::CspVersion()
    {
    return iCspVersion;
    }


//-------------------------------------------------------
   

// End of File
