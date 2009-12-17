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
* Description: Http Transaction class for Imps.
*
*
*/


#ifndef IMPSHTTPTRANSACTION_H
#define IMPSHTTPTRANSACTION_H

// INCLUDES
#include "http.h"
#include "ImpsTransportTimerCallback.h"

//CONSTANTS
const TInt KMaxNumberOfRetries              = 3;
_LIT8( K8BitRequestPost,                    "POST" );

//FORWARD DECLARATIONS
class CImpsTransportTimer;
class CHttpTransportAdapter;

// CLASS DECLARATION
class CImpsHttpTransaction : public CBase,
            public MHTTPDataSupplier,
            public TImpsTimingRequester
    {
    public:  // Constructors and destructor

        static CImpsHttpTransaction* NewL( CHttpTransportAdapter* aTransportAdapter,
                                           const TInt aTID,
                                           const TDesC8& aRequestBody );

        /**
        * Destructor.
        */
        virtual ~CImpsHttpTransaction();

        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void CopyRequestDataL( const TDesC8& aRequestData );

        /**
        * Copies the request data
        * @param aRequestData Data to be sent
        */
        void ConstructRequestL();

        /**
        * Complete this request
        * @param aErrorCode The completion code
        */
        void FinaliseRequestL( const TInt aErrorCode );

        /**
        * Delete the request data
        * @return void
        */
        void DoReleaseData();

        /**
        * Sets the status of this transaction
        * @param aSent Has this transaction been sent or not
        */
        void SetStatus( const TBool aSent );

        /**
        * Return the completion status of this transaction
        * @return TInt HTTP Status of the transaction
        */
        const TInt HttpStatus() const;

        /**
        * Try to resend this request
        * @return TBool Resent or not
        */
        TBool ResendL();

        /**
        * Send this transaction to be handled by the HTTP framework.
        */
        void DispatchMessageL();

        /**
        * Sets the expiry time of this transaction
        * @param aExpiryTime Time period after which this transaction expires
        * @param aCallback Handle to the receiver of the expiry event
        */
        void SetExpiryTimeL( const TTimeIntervalMicroSeconds32 aExpiryTime,
                             MImpsTransportTimerCallback* aCallback );

        /**
        * Append data to the response buffer.
        */
        void AppendDataL( const TPtrC8& aBodyPart,
                          const TBool aLastChunk );

        /**
        * Cancel this transaction
        */
        inline void Cancel();

        /**
        * Has this transaction been cancelled.
        * @return TBool
        */
        inline TBool IsCancelled();

        /**
        * Has this transaction been sent
        * @return TBool
        */
        inline TBool Status() const;

        /**
        * Returns the TID of this transaction
        * @return TInt
        */
        inline const TInt TID() const;

        /**
        * Return the HTTP transaction associated with this transaction
        * @return RHTTPTransaction
        */
        inline RHTTPTransaction Transaction() const;

        /**
        * Return the time stamp of this transaction
        * @return TInt
        */
        inline const TInt SendTime() const;

        /**
        * Return the time stamp of this transaction
        * @return TInt
        */
        inline const TBool LastChunk() const;

        /**
        * Set the Content-Length of the response
        * @param TInt aContentLength
        * @return void
        */
        inline void SetContentLength( const TInt aContentLength );

#ifdef _DEBUG

        /**
        * Get the current time (System tick count)
        */
        TInt TimeL() const;

#endif

    private:

        /**
        * Second-phase constructor.
        */
        void ConstructL( const TDesC8& aRequestBody );

        /**
        * C++ default constructor is private.
        */
        CImpsHttpTransaction( CHttpTransportAdapter* aTransportAdapter,
                              const TInt aTID );

        /**
        * Returns the overall size of the payload data. This is part
        * of MHTTPDataSupplier interface. Thus, this function is only
        * called by the HTTP Stack
        * @return TInt
        */
        inline TInt OverallDataSize();

        /**
        * From MHTTPDataSupplier Supplies the HTTP stack with the payload data
        * @param aDataPart Upon return, contains the data to be sent
        * @return Did we just supply the last chunk of data
        */
        TBool GetNextDataPart( TPtrC8& aDataPart );

        /**
        * The transaction has been sent, now release the data
        * Part of MHTTPDataSupplier interface.
        */
        void ReleaseData();

        /**
        * Resets the transaction. Part of MHTTPDataSupplier interface.
        * NOT USED, but needs to be declared, since the function has been
        * defined as pure virtual in the base class
        * @return KErrNotSupported
        */
        inline TInt Reset();

    private: //Data

        HBufC8*                                 iRequestData;
        CBufSeg*                                iResponseData;
        CImpsTransportTimer*                    iExpiryTimer;
        CHttpTransportAdapter*                  iTransportAdapter;
        RHTTPTransaction                        iHttpTransaction;
        const TInt                              iTID;
        TInt                                    iNumberOfRetries;
        TInt                                    iSendTime;
        TInt                                    iContentLength;
        TInt                                    iCurrentDataLength;
        TTimeIntervalMicroSeconds32             iExpiryTime;
        TBool                                   iSent;
        TBool                                   iCancelled;
        TBool                                   iConstructed;
        TBool                                   iLastChunk;
    };

#include "ImpsHttpTransaction.inl"

#endif

// End of File
