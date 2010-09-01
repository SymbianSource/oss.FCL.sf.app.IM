/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* A manager Class for CIR Channel.  
*
*/


#ifndef CImpsCirManager_H
#define CImpsCirManager_H

//  INCLUDES
#include <e32base.h>
#include <msgconnmanagerapi.h>
#include "impsipcirwatcherapi.h"
#include "impscspsessionapi.h"

// CONSTANTS
const TUint KImpsCirTcp = 0x0001;
const TUint KImpsCirUdp = 0x0002;

// MACROS

// DATA TYPES

// Connection states
enum TImpsCirState
    {
    EImpsCirOnline = 0,
    EImpsCirOffline,
    EImpsCirOpeningTcp
    };

typedef TUint TImpsCirChannels;

// FORWARD DECLARATIONS
class CImpsSendReceive2;
class MMsgConnManager;

// CLASS DECLARATION

class CImpsCirManager : public CActive, public MImpsCirReceiver
    {
    public: // new methods

        /**
        * Two phase constructor
        * @param aServer WV engine server
        * @param aPriority CActive priority
        * @return CImpsCirManager entity
        */
        static CImpsCirManager* NewL( MImpsCSPSession& aServer, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsCirManager();

        /**
        * Open CIR channels
        * @param aChannels channel bindings
        * @param aManager Connection Manager
        * @param aServerPort TCP port
        * @param aServerAddress TCP server address
        * @param aCSPSessionID current CSP session ID
        * @param aUDPPOrt UDP port
        */
        void OpenChL(
            TImpsCirChannels aChannels,
            MMsgConnManager& aManager,
            TUint aServerport,
            const TDesC& aServerAddress,
            const TDesC8& aCSPSessionID,
            TUint aUDPPort = KDefaultUDPPortNumber );

        /**
        * Close CIR channels
        * @param aChannels channel bindings
        */
        void CloseCh( TImpsCirChannels aChannels );

        /**
        * CIRWatcher calls this method when it receives a CIR
        * from the remote server.
        * @param aMessage received CIR message
        * @param sCirType CIR channel
        */

    public:     // From base classes

        void CirReceivedL( const TDesC8& aMessage,
                           const TImpsSupportedCirTypes aCirType );

        /**
        * CIRWatcher calls this method in case it encounters an
        * error situation from which it cannot recover. In general,
        * the watcher is designed to be as self-contained as possible,
        * but it can still happen that something quite unexpected
        * happens that forces the client to handle the situation by,
        * for instance, stopping the whole service and reinstating
        * the watcher after the erroneus conditions have been dealt with.
        * This could be called an emergency brake of a sort.
        */
        void HandleErrorL( const TInt aReceiverError,
                           const TImpsSupportedCirTypes aCirType );


    protected:
        // From base class
        void DoCancel();
        void RunL();

    private:

        /**
        * Open CIR TCP channel
        * @param aServerPort TCP port
        * @param aServerAddress TCP server address
        * @param aCSPSessionID current CSP session ID
        */
        void DoOpenTCPL(
            TUint aServerport,
            const TDesC& aServerAddress,
            const TDesC8& aCSPSessionID );


        /**
        * Open CIR UDP channel
        * @param aUDPPOrt UDP port
        */
        void DoOpenUDP(
            TUint aUDPPort = KDefaultUDPPortNumber );

        /**
        * By default constructor is private.
        */
        void ConstructL(  );

        /**
        * Constructor
        * @param aServer WV engine server
        * @param aPriority Active objects priority
        */
        CImpsCirManager( MImpsCSPSession& aServer, TInt aPriority );

        /**
        * Activate entity
        * @return error code KErrNotReady if previous request incomplete
        */
        TInt ActivateMe();

        /**
        * Error handling
        * @param aReceiverError error code
        * @param aCirType CIR channel
        */
        void DoHandleError( const TInt aReceiverError,
                            const TImpsSupportedCirTypes aCirType );


    private:
        MImpsCSPSession&            iServer;
        TImpsCirState               iState;
        TBool                       iCanceled;
        MMsgConnManager*            iManager;
        CImpsTcpCirWatcher*         iTCPWatcher;
        CImpsUdpCirWatcher*         iUDPWatcher;
        RLibrary                    iLibrary;
        TImpsCirChannels            iChannels;
        TImpsCirChannels            iReqChannels;
        TUint                       iUDPPort;
    };


#endif      // ?INCLUDE_H   

// End of File
