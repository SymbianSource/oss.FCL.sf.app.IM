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
* Description:  Storage main client
*
*/

#ifndef __RPENGSTORAGECLIENT_H__
#define __RPENGSTORAGECLIENT_H__

//  INCLUDES
#include <e32std.h>

/**
 *  Storage Main client
 *  It is used for connecting of the storage sub-clients
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */

// CLASS DECLARATION
NONSHARABLE_CLASS(  RPEngStorageClient ):
        public RSessionBase
    {

public:     // Constructors and destructors
    /**
     * Destructor.
     */
    ~RPEngStorageClient();

    /**
     * C++ default constructor.
     */
    RPEngStorageClient();

public: // Connect close methods

    /**
     * Connects to the presence server
     *
     *  @since 3.0
     *  @return KErrNone if connected
     *       or any of system wide error codes
     */
    TInt Connect();

    /**
     *  Close connection to the presence server
     *
     *  @since 3.0
     */
    void Close();

public: // new methods for Client usage

    /**
     *  Get Session Slot state
     *
     *  @since 3.0
     *  @param aSessionName packed session slot Id
     *  @param aSessionState on returns contain packed session slot
     *          state
     *  @return KErrNone if request went through, or system wide error
     */
    TInt SessionSlotState( const TDesC8& aSessionName,
                           HBufC8*& aSessionState,
                           TInt aInitSize );

    /**
     *  Get list of States of all known Session Slots
     *
     *  @since 3.0
     *  @param aSessionSlotsBuffer on return contact packed session
     *              slot states of all available session slots
     *  @return KErrNone if request went through, or system wide error
     */
    TInt AllSessionSlotsStates( HBufC8*& aSessionSlotsBuffer,
                                TInt aInitSize );

    /**
     *  Start listening global Events of the storage manager
     *
     *  @since 3.0
     *  @return KErrNone if request went through, or system wide error
     */
    TInt ListenGlobalEvents();

    /**
     *  Reload global listener
     *
     *  @since 3.0
     *  @param aGlobalEventBuffer buffer new event will be written
     *          when it occures.
     *  @param aStatus TRequestStatus for reloading
     *  @return KErrNone if request went through, or system wide error
     */
    TInt ReloadGlobalEventListener( TDes8& aGlobalEventBuffer,
                                    TRequestStatus& aStatus );

    /**
     *  Stop global events listener
     *
     *  @since 3.0
     *  @return KErrNone if request went through, or system wide error
     */
    TInt StopEventListening();

    /**
     *  Create Session Sub-folder to the storage manager store
     *
     *  @since 3.0
     *  @param aSessionName packed session slot Id
     *  @param aApplicationId application Id
     *  @return KErrNone if request went through, or system wide error
     */
    TInt CreateSessionFolder( const TDesC8& aSessionSlot,
                              const TDesC16& aApplicationId );

    /**
     *  Remove Session Sub-folder in the storage manager store
     *
     *  @since 3.0
     *  @param aSessionName packed session slot Id
     *  @param aApplicationId application Id
     *  @return KErrNone if request went through, or system wide error
     */
    TInt RemoveSessionFolder( const TDesC8& aSessionSlot,
                              const TDesC16& aApplicationId );

private:    // New functions

    /**
     * Get version
     * @since 3.0
     * @return version of the client
    */
    TVersion Version() const;

private:    // Data

    /// Flag if client is connected to the Storage Server
    TBool                   iConnected ;

    };

#endif      //  __RPENGSTORAGECLIENT_H__



