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
* Description: Http Transaction class for Imps.
*       
*
*/


// ----------------------------------------------------------
// CImpsHttpTransaction::Status
// 
// ----------------------------------------------------------
//
inline TBool CImpsHttpTransaction::Status() const
	{
	return iSent;
	}

// ----------------------------------------------------------
// CImpsHttpTransaction::Cancel
// 
// ----------------------------------------------------------
//
inline void CImpsHttpTransaction::Cancel()
	{
	iCancelled = ETrue;
	}

// ----------------------------------------------------------
// CImpsHttpTransaction::SetContentLength
// 
// ----------------------------------------------------------
//
inline void CImpsHttpTransaction::SetContentLength( const TInt aContentLength )
    {
    if( aContentLength >= 0 && aContentLength < KMaxTInt )
        iContentLength = aContentLength;
    }

// ----------------------------------------------------------
// CHttpTransportAdapter::OverallDataSize
// MHTTPDataSupplier interface callback. 
// ----------------------------------------------------------
//
inline TBool CImpsHttpTransaction::IsCancelled()
	{
	return iCancelled;
	}

// ----------------------------------------------------------
// CImpsHttpTransaction::Reset
// MHTTPDataSupplier interface callback
// ----------------------------------------------------------
//
inline TInt CImpsHttpTransaction::Reset()
	{
	return KErrNotSupported;
	}

// ----------------------------------------------------------
// CImpsHttpTransaction::OverallDataSize
// MHTTPDataSupplier interface callback
// ----------------------------------------------------------
//
inline TInt CImpsHttpTransaction::OverallDataSize()
	{
    return iRequestData != NULL ? iRequestData->Length() : 0;
	}

// ----------------------------------------------------------
// CImpsHttpTransaction::TID
//
// ----------------------------------------------------------
//
inline const TInt CImpsHttpTransaction::TID() const
    {
	return iTID;
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::TID
//
// ----------------------------------------------------------
//
inline const TBool CImpsHttpTransaction::LastChunk() const
    {
    return iLastChunk;
    }

// ----------------------------------------------------------
// CImpsHttpTransaction::TID
//
// ----------------------------------------------------------
//
inline const TInt CImpsHttpTransaction::SendTime() const
    {
	return iSendTime;
    }

// ----------------------------------------------------
// CImpsHttpTransaction::Transaction
//
// ----------------------------------------------------
//
inline RHTTPTransaction CImpsHttpTransaction::Transaction() const
    { 
    return iHttpTransaction;
    }
