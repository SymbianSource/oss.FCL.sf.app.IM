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
* Description: Implements the CIMStatusFetcher and the MImStatusList interface.
*
*/



#ifndef IMAPI_IMSTATUSIMPL_H
#define IMAPI_IMSTATUSIMPL_H

//  INCLUDES
#include "imstatus.h"
#include "impspresencecli.h"
#include "impspresence.h"
#include "loginmngr.h"


// FORWARD DECLARATIONS
class CImConnectionImpl;

// CLASS DECLARATION

/**
*  Implements the CIMStatusFetcher interface
*
*  @lib
*  @since S60 2.6
*/
class CIMStatusFetcherImpl : public CBase,
            public MImStatusFetcher,
            public MImpsPresenceHandler,
            public MImpsErrorHandler
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIMStatusFetcherImpl* NewL(
            CImConnectionImpl* aImConnection );
        /**
        * Destructor.
        */
        virtual ~CIMStatusFetcherImpl();

    public: // New functions

        MImpsPresenceHandler* ImpsPresenceHandler();

        MImpsErrorHandler& ImpsErrorHandler();

        RImpsEng& ImpsEngine();

        RImpsPresenceClient& ImpsPresenceClient();

        TDesC& ClientId();

        MImStatusObserver* ClientObserver();

    public: // Functions from base classes

        /**
        * From  CIMStatusFetcher
        * @see CIMStatusFetcher::RegisterObserverL
        */
        virtual TInt RegisterObserverL(
            MImStatusObserver* aObserver );

        /**
        * From  CIMStatusFetcher
        * @see CIMStatusFetcher::UnregisterObserverL
        */
        virtual TInt UnregisterObserverL( );

        /**
        * From  CIMStatusFetcher
        * @see CIMStatusFetcher::GetOnlineStatusL
        */
        virtual TInt GetOnlineStatusL(
            const CContactIdArray& aContactIds );

        /**
        * From  CIMStatusFetcher
        * @see CIMStatusFetcher::GetOnlineStatusL
        */
        virtual TInt GetOnlineStatusL(
            const CDesCArray& aUserIds );

        /**
        * From MImpsErrorHandler
        * @see MImpsErrorHandler::HandleErrorL
        */
        virtual void HandleErrorL(
            TInt                 aStatus,
            TInt                 aOpId,
            const TDesC*         aDescription,
            const CImpsDetailed* aDetailedRes );

        /**
        * From MImpsPresenceHandler
        * @see MImpsPresenceHandler::HandleNewPresenceL
        */
        virtual void HandleNewPresenceL(
            TInt                        aStatus,
            TInt                        aOpId,
            CImpsPresenceList&          aPresList );


        /**
        * From MImpsPresenceHandler
        * @see MImpsPresenceHandler::HandleCompleteL
        */
        virtual void HandleCompleteL(
            TInt aStatus,
            TInt aOpId );

    private:

        /**
        * C++ default constructor.
        */
        CIMStatusFetcherImpl(
            CImConnectionImpl* aImConnection );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Internal state enum of the class
        enum TRegState
            {
            ENotRegistered = 0,
            ERegistering,
            ERegistered
            };

        void ChangeRegState( TRegState aNewRegState );

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    private:    // Data
        RImpsPresenceClient   iPresenceClient; // Imps Presence client
        MImStatusObserver*    iClientObserver; // The 3rd party IM observer (not owned)
        CImConnectionImpl*    iImConnection;// The API manager (not owned)
        HBufC*                iClientId;// Client ID of the 3rd party app (owned)


    };


// CLASS DECLARATION

/**
*  Implements the MImStatusList interface
*
*  @lib
*  @since S60 2.6
*/
class CIMStatusListImpl : public CBase,
            public MImStatusList
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CIMStatusListImpl* NewL(
            CImpsPresenceList& aPresList );

        /**
        * Destructor.
        */
        virtual ~CIMStatusListImpl();

    public: // Functions from base classes
        TInt Count();

//        TContactItemId& OwnerContactIdL( TInt aIndex );

        TPtrC OwnerUserIdL( TInt aIndex );

        TIMAttribute IMStatusL( TInt aIndex );


    private:
        /**
        * C++ default constructor.
        */
        CIMStatusListImpl( CImpsPresenceList& aPresList );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //DATA
        CImpsPresenceList&      iPresList;
        CImpsPresence*          iCurrentPresence;
        TInt                    iCurrentIndex;

    };


#endif      // IMAPI_IMSTATUSIMPL_H   

// End of File
