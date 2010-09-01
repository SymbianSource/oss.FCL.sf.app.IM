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
* Description:  Sub-session class of the Storage Server
*
*/



#ifndef __CPENGSTORAGESUBSESSION_H__
#define __CPENGSTORAGESUBSESSION_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "TPEngStorageServerMessages.h"

// FORWARD DECLARATIONS
class MPEngStorageServer;
class RPEngMessage;
class CPEngStorageFolder;
class CPEngDataEntry;

/**
 *  Subsession of the Storage Server
 *  @since 3.0
 */
class CPEngStorageSubSession : public CObject
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPEngStorageSubSession* NewL( MPEngStorageServer& aServer,
                                             const RPEngMessage& aMessage,
                                             TInt32 aSessionId );

        /**
        * Two-phased constructor.
        */
        static CPEngStorageSubSession* NewLC( MPEngStorageServer& aServer,
                                              const RPEngMessage& aMessage,
                                              TInt32 aSessionId );

        /**
        * Destructor.
        */
        virtual ~CPEngStorageSubSession();

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
        void PanicClient( const RPEngMessage& aMessage, const TInt aPanic ) const;


        /**
         *	Resume Notification of the SIDs
         *
         *	@since 3.0
         *	@param aCount count do decrease Notification lock counter
         */
        void ResumeSIDsNotification( TInt aCount );

    private:

        /**
        * C++ constructor.
        */
        CPEngStorageSubSession( TInt32 aSessionId );

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL( MPEngStorageServer& aServer, const RPEngMessage& aMessage );

    private:    // Data

        ///	OWN: Buffer of changed SIDs which notification was postponed
        RPointerArray<CPEngDataEntry>		iBufferedNotif;

        /// OWN: Counter of the buffering activation
        TInt								iBufferingActiveCounter;

        /// REF: Storage Cage
        CPEngStorageFolder*					iStorageFolder;

        /// OWN: Sub Session Handle
        TInt								iHandle;

        /// OWN: Session Id
        const TInt32                        iSessionId;

    };

#endif      // __CPENGSTORAGESUBSESSION_H__

// End of File
