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
* Description: CirWatcher for UDP
*
*
*/


#ifndef __UDPCIRWATCHER__
#define __UDPCIRWATCHER__

// INCLUDES
#include <in_sock.h>
#include <es_sock.h>
#include "ImpsIpCirWatcherAPI.h"



//FORWARD DECLARATIONS
class CUdpSocketListener;
class MImpsConnectionManager;

// CLASS DECLARATION
class CUdpCirWatcher : public CImpsUdpCirWatcher

    {
    public:  // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        static CUdpCirWatcher* NewL();

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        void RegisterCirReceiverL( const MImpsCirReceiver& aCirReceiver,
                                   const TInt aServicePort = KDefaultUDPPortNumber );

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        void UnregisterCirReceiverL( const MImpsCirReceiver& aCirReceiver );

        /**
        * Reset all watchers on a specified port. By default resets
        * all watchers on all ports!
        * @param TInt The port to reset; by default all ports
        * @return void
        */
        void Reset( const TInt aServicePort = KErrArgument );

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        virtual void DeleteSingleton();

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        virtual void Destroy();

        /**
        * Destructor.
        */
        virtual ~CUdpCirWatcher();

    public:   //New functions

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        static void WriteToLog( TRefByValue<const TDesC8> aFmt, ... );

    private:

        /**
        * Symbian OS second-phase constructor.
        */
        void ConstructL();

        /**
        * C++ default constructor is private
        */
        CUdpCirWatcher();

        /**
        * C++ default constructor is private
        */
        void DeleteLogFileL();

        /**
        * Increase the number of clients for this singleton
        * @return void
        */
        void IncreaseReferenceCount();

        /**
        * Return the current reference count for the singleton object.
        * Comes in handy at least in testing.
        * @return TInt Current reference count
        */
        void DecreaseReferenceCount();

        /**
        * Return the current reference count for the singleton object.
        * Comes in handy at least in testing.
        * @return TInt Current reference count
        */
        const TInt ReferenceCount() const;

        /**
        * Return the current reference count for the singleton object.
        * Comes in handy at least in testing.
        * @return TInt Current reference count
        */
        void StoreDestructorKeyL( const TUid aDestructorKey );

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        TBool FindListenerL( const TInt aServicePort,
                             const MImpsCirReceiver& aCirReceiver );

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        CUdpSocketListener* FindListener( TInt& aIndex,
                                          const TInt aServicePort ) const;

        /**
        * Symbian OS default constructor.
        * @param aPingInterval Interval of the ping requests
        * @param aConnManager Reference to the active connection manager
        */
        TInt CreateListenerL( const TInt aServicePort,
                              const MImpsCirReceiver& aCirReceiver );

    private: //Data

        TInt                                        iReferenceCount;
        RSocketServ                                 iSocketSession;
        RPointerArray<CUdpSocketListener>           iListenerQueue;
    };

#endif

// End of File
