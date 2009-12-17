/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Base class for client side event handlers.
*
*/


#ifndef IMPSHANDLER_H
#define IMPSHANDLER_H

// INCLUDES
#include <e32base.h>
#include "impspacked.h"
#include "impsclientsrv.h"
#include "impshandlertimer.h"

// FORWARD DECLARATIONS
class RImpsClient2;
class MImpsStatusHandler2;
class MImpsErrorHandler2;
class CImpsFields;

// CLASS DECLARATION


// -----------------------------------------------------------------------------
// TWvBuf::CWvBuf
// -----------------------------------------------------------------------------
class TWvBuf 
    {
    public:
        /**
        * Constructor
        * @param aBuf data buffer
        */
        TWvBuf( HBufC8* aBuf );

    public:
        TPtr8  iPtr;    // pointer to the buffer
    };


/**
*  Abstract base class for WV event handlers.
*/
class CImpsHandler2 :public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CImpsHandler2();


    public: // New functions
        
        /** 
        * Start the execution. Ready to get messages after that.
        */
        void StartRun();

        /**
        * Session status handler accessor
        * @return status event handler
        */
        inline MImpsStatusHandler2* StatusHandler();

        /**
        * Session status handler mutator.
        * Registers status to server too.
        * @param aObserver status event observer
        */
        void SetStatusHandlerL( MImpsStatusHandler2* aObserver );

        /**
        * Error handler accessor
        * @return error handler
        */
        inline MImpsErrorHandler2* ErrorHandler();

        /**
        * Error handler mutator.
        * Registers status to server too.
        */
        void SetErrorHandlerL( MImpsErrorHandler2* aObserver );

        /**
        * Request event message body from the server
        */
        void EventHandled();

    private: // New functions

        /**
        * Read message from server thread
        * @param aLength data length to be read
        * @return ETrue if message read complete, no need to read body separately
        */
        TBool ReadDataL( TInt aLength );

        /**
        * Request event message body from the server
        */
        void EventBody();

        /**
        * Reset IPC buffer
        */
        void ResetBuffers();

        /**
        * Current event message buffer size
        * @return buffer size
        */
        inline TInt CurrentSize() const;
        
    protected:  // New functions

        /**
        * Actual construction.
        */
        void ConstructL();

        /**
        * Call appropriate observer
        * @param aErrorFields used if error case. NULL or ImpsFields structure
        *
        */
        virtual void HandleEventL( CImpsFields* aErrorFields )=0;

        /**
        * Detailed error handling
        * @param aOpCode operation-id
        * @param aStatus error code
        * @param aFields error data
        */
        void HandleErrorEventL(
            TInt aOpCode, TInt aStatus, CImpsFields* aFields);

        /**
        * Registers base class observer for session status change observer
        */
        void DoRegisterStatusHandlerL(  );

        /**
        * Unregisters base class observer for session status change observer
        */
        void DoUnregisterStatusHandlerL(  );

        /**
        * Registers base class observer for detailed error observer
        */
        void DoRegisterErrorHandlerL(  );

        /**
        * Unregisters base class observer for detailed error observer
        */
        void DoUnregisterErrorHandlerL(  );
        
    protected:  

        /**
        * C++ constructor 
        */
        CImpsHandler2( TInt aPriority, RImpsClient2& aClient );


    protected:  // Functions from base classes

        /**
        * From CActive 
        */
        void RunL();

        /**
        * From CActive 
        */
        void DoCancel();
  
    protected:  // Data

        // Message header area
        TBuf<sizeof(SImpsEventData)> iEventData;

        // Client interface reference who is using this object
        RImpsClient2&            iClient;
        // Pointer to registered online status observer
        MImpsStatusHandler2*     iStatusHandler;
        // Pointer to registered detailed error observer
        MImpsErrorHandler2*      iErrorHandler;
        // Internal message data structure
        CImpsFields*            iFields;

    private:    // Data

        // Pointer to stack variable to detect the deletion of the heap 
        // instance. This case takes place if a client
        // calls Unregister in observer method.
        TBool*                  iDestroyedPtr;

        // IPC buffers and auxiliary classes
        TIpcArgs                iArgs;
        HBufC8*                 iBody;
        TWvBuf*                 iPtrStore;
        
        // Timer for server busy situations
        CImpsHandlerTimer*		iBusyTimer;
    };


inline MImpsStatusHandler2* CImpsHandler2::StatusHandler()
    {
    return iStatusHandler;
    }

inline MImpsErrorHandler2* CImpsHandler2::ErrorHandler()
    {
    return iErrorHandler;
    }

inline TInt CImpsHandler2::CurrentSize() const
    {
    return iBody ? iBody->Des().MaxSize() : 0;
    }

#endif        
            
// End of File
