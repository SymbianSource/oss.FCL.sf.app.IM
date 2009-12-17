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
* Description: Adapter for Http transport.
*       
*
*/


// ----------------------------------------------------
// CHttpTransportAdapter::Destroy
// 
// ----------------------------------------------------
//
void CHttpTransportAdapter::Destroy()
    {
    delete this;
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::PendingRequests
// Return the number of pending requests 
// ----------------------------------------------------------
//
inline TInt CHttpTransportAdapter::PendingRequests() const
    {
    return iTransactionQueue.Count();
    }

// ----------------------------------------------------
// CHttpTransportAdapter::MimeType
// Return the MIME type of messages (HTTP header)
// ----------------------------------------------------
//
inline TPtrC8 CHttpTransportAdapter::MimeType() const
    {
    return iMimeBuffer != NULL ? iMimeBuffer->Des() : TPtrC8();
    }

// ----------------------------------------------------
// CHttpTransportAdapter::MimeType
// Return the MIME type of messages (HTTP header)
// ----------------------------------------------------
//
inline TBool CHttpTransportAdapter::SessionClosed() const
    {
    return iSessionClosed;
    }

// ----------------------------------------------------
// CHttpTransportAdapter::DefaultSAP
// Return the address of the default SAP 
// ----------------------------------------------------
//
inline TUriParser8 CHttpTransportAdapter::DefaultSAP() const
    {
    return iDefaultURL;
    }

// ----------------------------------------------------
// CHttpTransportAdapter::DefaultSAP
// Returns a reference to the currently active HTTP session 
// ----------------------------------------------------
//
inline RHTTPSession& CHttpTransportAdapter::HttpSession()
    {
    __ASSERT_ALWAYS( !iSessionClosed, User::Panic( _L( "CHttpTransportAdapter" ), 1 ) );
    return iHttpSession;
    }

// ----------------------------------------------------
// CHttpTransportAdapter::DefaultSAP
// Returns a reference to the currently active HTTP session 
// ----------------------------------------------------
//
inline MImpsDataReceiver& CHttpTransportAdapter::ReceiverHandle() const
    {
    return iReceiver;
    }
