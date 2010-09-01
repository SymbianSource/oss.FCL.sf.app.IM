/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the IM Client interface (imclient.h)
*
*/



#ifndef IMAPI_IMCLIENTIMPL_H
#define IMAPI_IMCLIENTIMPL_H

//  INCLUDES
#include "imclient.h"
#include "impsimcli.h"


// FORWARD DECLARATIONS
class CImConnectionImpl;

// CLASS DECLARATION
/**
*  Implementation of the MImClient abstract interface
*
*
*  @lib imclient.lib
*  @since S60 3.0
*/
class CImClient :   public CBase,
            public MImClient,
            public MImpsImHandler2,
            public MImpsErrorHandler2
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aImConnection the Connection
        */
        static CImClient* NewL( CImConnectionImpl& aImConnection );

        /**
        * Destructor.
        */
        virtual ~CImClient();

    public: // New functions

        /**
        * Gets the Imps IM handler
        * @since S60 3.0
        * @return MImpsImHandler
        */
        MImpsImHandler2* ImpsImHandler();

        /**
        * Gets the Imps Error handler
        * @since S60 3.0
        * @return MImpsErrorHandler
        */
        MImpsErrorHandler2& ImpsErrorHandler();

        /**
        * Gets the Imps Engine
        * @since S60 3.0
        * @return RImpsEngine
        */
        RImpsEng& ImpsEngine();

        /**
        * Gets the 3rd party IM observer
        * @since S60 3.0
        * @return MImObserver
        */
        MImObserver* ClientObserver();

    public: // Functions from base classes

        /**
        * From MImClient
        * @see MImClient::RegisterL
        */
        virtual void RegisterObserverL(
            MImObserver* aObserver );

        /**
        * From MImClient
        * @see MImClient::UnregisterL
        */
        virtual void UnregisterObserver( );

        /**
        * From MImClient
        * @see MImClient::SendPToPMessage
        */
        virtual TInt SendPToPMessageL(
            const TContactItemId& aContactItem,
            const TDesC16& aContent );

        /**
        * From MImClient
        * @see MImClient::SendPToPMessage
        */
        virtual TInt SendPToPMessageL(
            const CContactIdArray&  aContactIds,
            const TDesC&            aContentType,
            const TDesC8&           aContent );

        /**
        * From MImClient
        * @see MImClient::SendPToPMessage
        */
        virtual TInt SendPToPMessageL(
            const MDesCArray&   aUserIds,
            const TDesC&        aContentType,
            const TDesC8&       aContent );

        /**
        * From MImClient
        * @see MImClient::SendPToPMessage
        */
        virtual TInt SendPToPMessageL(
            const TDesC& aUserId,
            const TDesC16& aContent );

        /**
        * From MImClient
        * @see MImClient::SendPToPMessage
        */
        virtual TInt SendPToPMessageL(
            const CContactIdArray&  aContactIds,
            const TDesC16&          aContent );

        /**
        * From MImClient
        * @see MImClient::SendPToPMessage
        */
        virtual TInt SendPToPMessageL(
            const MDesCArray&   aUserIds,
            const TDesC16&      aContent );
        /**
        * From MImpsImHandler2
        * @see MImpsImHandler2::HandleNewTextMessageL
        */
        virtual void HandleNewTextMessageL(
            TInt                aOpId,
            const TDesC&        aMessageId,
            const TDesC&        aSender,
            const TDesC&        aGroupId,
            const MDesCArray&   aRecipients,
            const MDesCArray&   aScreenNames,
            const TDesC&        aText,
            TImpsCspIdentifier& aCspId );

        /**
        * From MImpsImHandler2
        * @see MImpsImHandler::HandleSendCompleteL
        */
        virtual void HandleSendCompleteL(
            TInt                aOpId,
            TBool               aDeliveryReportOrdered,
            TImpsCspIdentifier& aCspId );

        /**
        * From MImpsImHandler2
        * @see MImpsImHandler2::HandleNewContentMessageL
        */
        virtual void HandleNewContentMessageL(
            TInt aOpId,
            const TDesC& aMessageId,
            const TDesC& aSender,
            const TDesC& aGroupId,
            const MDesCArray& aRecipients,
            const MDesCArray& aScreenNames,
            const TDesC& aContentType,
            const TDesC8& aContent,
            TImpsCspIdentifier& aCspId );

        /**
        * From MImpsImHandler2
        * @see MImpsImHandler2::HandleDeliveryReportL
        */
        virtual void HandleDeliveryReportL(
            const TDesC& aMessageId,
            TInt aResult,
            const TDesC* aDescription,
            TImpsCspIdentifier& aCspId );

        /**
        * From MImpsErrorHandler
        * @see MImpsErrorHandler::HandleErrorL
        */
        virtual void HandleErrorL(
            TInt                 aStatus,
            TInt                 aOpId,
            const TDesC*         aDescription,
            const CImpsDetailed* aDetailedRes,
            TImpsCspIdentifier&  aCspId );


    private:

        /**
        * C++ default constructor.
        * @param aImConnection the Connection
        */
        CImClient( CImConnectionImpl& aImConnection );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Sends IM using WV client
        * @param aUserIds the recipients list
        * @param aContentType the content type of the message
        * @param aContent the message
        * @return OpId
        */
        TInt DoSendMessageL(
            const MDesCArray&   aUserIds,
            const TDesC&        aContent );


        /**
        * Deletes the OpId from the outstanding OpIds list
        * @param aOpId an OpId to be deleted
        */
        void RemoveOpId( TInt aOpId );


    private:    // Data
        CImConnectionImpl&  iImConnection;  // The API manager (not owned)
        RImpsImClient2      iImClient;      // Imps IM client
        MImObserver*        iClientObserver;// The user observer (not owned)
        RArray<TInt>        iOutstandingOpIds; // OpIds waiting for response
    };

// CLASS DECLARATION
// Detailed result item received from the remote SAP
class TDetailedResultItem
    {
    public:
        TDetailedResultItem();
        ~TDetailedResultItem();

        /**
        * Error code setter
        * @param aErroCode the error code
        */
        void SetErrorCode( TInt aErroCode );

        /**
        * Error code getter
        * @return the error code
        */
        TInt ErrorCode( ) const;

        /**
        * User ID setter
        * @param aUserId the user ID
        */
        void SetUserId( TDesC& aUserId );

        /**
        * User ID getter
        * @return the user ID
        */
        const TDesC& UserId( ) const;

    private:
        TInt    iErrorCode;
        TPtrC   iUserId;
    };

/**
*  Implementation of the MImClientDetailedError abstract interface
*
*
*  @lib imclient.lib
*  @since S60 3.0
*/
class CImClientDetailedErrorImpl :  public CBase,
            public MImClientDetailedError
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aImConnection the Connection
        */
        static CImClientDetailedErrorImpl* NewL(
            const CImpsDetailed* aImpsDetailed );

        /**
        * Destructor.
        */
        virtual ~CImClientDetailedErrorImpl();

    public: // Functions from base classes

        /**
        * From MImClientDetailedError
        * @see MImClientDetailedError::Count
        */
        TInt Count();

        /**
        * From MImClientDetailedError
        * @see MImClientDetailedError::UserId
        */
        const TDesC& UserId( TInt aIndex );

        /**
        * From MImClientDetailedError
        * @see MImClientDetailedError::ErrorCode
        */
        TInt ErrorCode( TInt aIndex );

    private:
        CImClientDetailedErrorImpl( );
        void ConstructL( const CImpsDetailed* aImpsDetailed );

        // Constructs the object from CImpsDetailed
        void DoInitL( const CImpsDetailed* aImpsDetailed );

        /**
        * Converts the error code returned by the WV server to API error code.
        * @since 3.0
        * @param aErroCode WV error code
        * @return API error code
        */
        TInt ConvertWVErrorCode( TInt aErrorCode );


    private: //Data
        CArrayFixFlat<TDetailedResultItem>*  iDetailedList;

    };


#endif      // IMAPI_IMCLIENTIMPL_H   

// End of File
