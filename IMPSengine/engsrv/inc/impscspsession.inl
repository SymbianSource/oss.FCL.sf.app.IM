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
* Description: Inl file for CImpsCSPSession inline functions
* 
*/


inline TBool CImpsCSPSession::DoIsLogged() const
    {
    return  ( iCSPState >= EImpsCSPLogged  ) ? ETrue : EFalse; 
    }

inline TBool CImpsCSPSession::DoIsLogging() const
    {
    return  ( iCSPState == EImpsCSPLogging  ) ? ETrue : EFalse; 
    }

inline TBool CImpsCSPSession::DoIsNegotiated() const
    {
    return  ( iCSPState >= EImpsCSPOnLine  ) ? ETrue : EFalse; 
    }

inline TPtrC CImpsCSPSession::DoUserId() const
    {
    return iUserId ? TPtrC( *iUserId ) : TPtrC();
    }

inline TBool CImpsCSPSession::DoIsShuttingDown() const
    {
    return iCSPState == EImpsCSPShuttingDown ? ETrue : EFalse;
    }

inline TPtrC CImpsCSPSession::LogoutTID() const
    {
    return iLogoutTID;
    }

inline TPtrC CImpsCSPSession::DoSAP() const
    {
    return iSAP ? TPtrC( *iSAP ) : TPtrC(); 
    }

inline TPtrC CImpsCSPSession::LogTid() const
    {
    return iLogTid ? TPtrC( *iLogTid ) : TPtrC();
    }

inline TPtrC CImpsCSPSession::LogPwd() const
    {
    return iLogPwd ? TPtrC( *iLogPwd ) : TPtrC();
    }

inline TBool CImpsCSPSession::IsPDPIdleOnLine() const
    {
    return iCSPState >= EImpsCSPOnLineIdle && iIntStatus == EInternal_ON_LINE ? 
        ETrue : EFalse;
    }

inline TBool CImpsCSPSession::IsPDPIdleOffLine() const
    {
    return iCSPState >= EImpsCSPOnLineIdle && iIntStatus == EInternal_OFF_LINE ? 
        ETrue : EFalse;
    }

inline TBool CImpsCSPSession::IsPDPIdle() const
    {
    return ( iCSPState >= EImpsCSPOnLineIdle ? ETrue : EFalse );
    }

