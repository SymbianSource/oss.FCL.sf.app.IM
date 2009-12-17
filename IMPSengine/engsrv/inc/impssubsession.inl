/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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


inline CImpsFields* CImpsSubSession::ImpsFields() const
    {
    return iSession->ImpsFields();
    }

inline HBufC8* CImpsSubSession::StreamBuf() const
    {
    return iSession->StreamBuf();
    }

inline HBufC8** CImpsSubSession::StreamBufAddr()
    {
    return iSession->StreamBufAddr();
    }

inline TBool CImpsSubSession::StatusObserver(  ) const
    {
    return iStatusObserver;
    }

inline TImpsEventType CImpsSubSession::Type() const
    {
    return iServiceType;
    }

inline void CImpsSubSession::NewFieldsL()
    {
    iSession->NewFieldsL();
    }

inline void CImpsSubSession::SetStatusObserver( TBool aVal )
    {
    iStatusObserver = aVal;
    }


inline void CImpsSubSession::SetDetailedError( TBool aVal )
    {
    iDetailedError = aVal;
    }

inline void CImpsSubSession::SetHandle( TUint aVal )
    {
    iSubHandle = aVal;
    }

inline TUint CImpsSubSession::Handle()
    {
    return iSubHandle;
    }

inline RMessagePtr2 CImpsSubSession::EventMsg()
    {
    return iEventMsg;
    }


