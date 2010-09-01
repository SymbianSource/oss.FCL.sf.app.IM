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
* Description:  Admin Sub-session class of the Storage Server
*
*/



#ifndef __CPENGADMINSUBSESSION_H__
#define __CPENGADMINSUBSESSION_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "TPEngStorageServerMessages.h"

// FORWARD DECLARATIONS
class MPEngStorageServer;
class RPEngMessage;
class CPEngStorageFolder;


/**
 *  Admin Subsession of the Storage Server
 *  @since 3.0
 */
class CPEngAdminSubSession : public CObject
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPEngAdminSubSession* NewL( MPEngStorageServer& aServer,
                                           const RPEngMessage& aMessage,
                                           TInt32 aSessionId );

        /**
        * Two-phased constructor.
        */
        static CPEngAdminSubSession* NewLC( MPEngStorageServer& aServer,
                                            const RPEngMessage& aMessage,
                                            TInt32 aSessionId );

        /**
        * Destructor.
        */
        virtual ~CPEngAdminSubSession();

    public: // Function for handling of the clients requests

        /**
          * Dispatch message from the Sub Session
          *
          *	@since 3.0
          *	@param aMessage is received message.
          *	@return ETrue if message should be completed
          */
        TBool DispatchMessageL( const RPEngMessage& aMessage,
                                TInt aRequest );

        /**
          * Set Sub-session handle ID for the Session
          *
          *	@since 3.0
          *	@param aHandle Sub-session handle
          */
        void SetSubSesionHandle( TInt aHandle );

    private:  // Functions from base classes

        /*
         *	Client is panicked using this method.
         *
         *	@since 3.0
         *	@param	aMessage clients message
         *	@param	aPanic panic reason
         */
        void PanicClient( const RPEngMessage& aMessage,
                          const TInt aPanic ) const;

        /**
         *  Notify about new global event
         *
         *  @since 3.0
         *  @param	aMessage clients message
         */
        inline void NotifyGlobalEventL( const RPEngMessage& aMessage );

        /**
         *  Notify Error to listeners
         *
         *  @since 3.0
         *  @param	aMessage
         */
        inline void NotifyError( const RPEngMessage& aMessage );

        /**
         *  Check application registration
         *
         *  @since 3.0
         *  @param aMessage request message
         */
        inline void CheckAppIdRegistrationL( const RPEngMessage& aMessage );

        /**
         *  Notify Global Session slot close
         *
         *  @since 3.0
         */
        inline void NotifySessionSlotCloseL();

    private:

        /**
        * C++ default constructor.
        */
        CPEngAdminSubSession( MPEngStorageServer& aServer, TInt32 aSessionId );

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL( const RPEngMessage& aMessage );

    private:    // Data

        /// REF: Storage Cage
        CPEngStorageFolder*					iStorageFolder;

        /// OWN: Sub Session Handle
        TInt								iHandle;

        /// OWN: Session Id
        const TInt32                        iSessionId;

        /// REF: Server reference
        MPEngStorageServer&					iServer;

    };

#endif      // __CPENGADMINSUBSESSION_H__

// End of File
