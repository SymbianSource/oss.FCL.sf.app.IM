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
* Description:  Inline methods
*
*/


inline CImpsServer* CImpsSession::Server()
    {
    return (CImpsServer*)( CSession2::Server() );
    }

inline void CImpsSession::SetCompleted( )
   {
   iMessageCompleted = ETrue;
   }

inline CImpsFields* CImpsSession::ImpsFields() const
    {
    return iFields;
    }

inline HBufC8* CImpsSession::StreamBuf() const
    {
    return iStream;
    }


inline HBufC8** CImpsSession::StreamBufAddr()
    {
    // Be careful with this buffer entity
    return &iStream;  //lint !e1536
    }


inline TBool CImpsSession::IsCIRWatcherSession() const
    {
    return iCIRSess;
    }

inline TInt CImpsSession::Types() const
    {
    return (TInt)iTypes;
    }

inline TInt CImpsSession::NbrSubs() const
    {
    return iResourceCount;
    }
    
inline RMessagePtr2 CImpsSession::Message() const
    {
    return iMsgR;
    }

inline TPtrC CImpsSession::UserId( ) const
    {
    return iUserId ? TPtrC( *iUserId ) : TPtrC();
    }

inline TPtrC CImpsSession::SAP( ) const
    {
    return iSAP ? TPtrC( *iSAP ) : TPtrC();
    }

inline TPtrC CImpsSession::SID( ) const
    {
    return iSID ? TPtrC( *iSID ) : TPtrC();
    }

inline TBool CImpsSession::IsThisLogged() const
    {
    return ( SID().Length() ) ? ETrue : EFalse;
    }
    
inline TBool CImpsSession::IsThisActive() const
    {
    return ( UserId().Length() ) ? ETrue : EFalse;
    }  
          

//-------------------------------------------------------
   

// End of File
