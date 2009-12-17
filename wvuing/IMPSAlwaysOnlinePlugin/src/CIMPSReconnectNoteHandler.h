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
* Description:  Shows reconnect continuation global confirmation query and
*                follows user answering
*
*/



#ifndef __CIMPSRECONNECTNOTEHANDLER_H
#define __CIMPSRECONNECTNOTEHANDLER_H

//  INCLUDES
#include "impspresenceconnectionuiconstsng.h"
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <barsc.h>

// FORWARD DECLARATIONS
class MIMPSReconnectNoteObserver;
class CAknGlobalConfirmationQuery;
class CIMPSPresenceAAConnection;

// CLASS DECLARATION

/**
*  class for showing global confirmation query for continuing reconnecting
*  @lib PEngAOPlugin.lib
*  @since 2.1
*/
class CIMPSReconnectNoteHandler : public CActive
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIMPSReconnectNoteHandler* NewL( TIMPSConnectionClient aClient );

        /**
        * Destructor.
        */
        virtual ~CIMPSReconnectNoteHandler();

    public: // New functions

        /**
         * Shows global note, if not already shown and adds observer
         * to listen answer for global note
         * @param aObserver Observer for global note
         */
        void ShowNote( MIMPSReconnectNoteObserver* aObserver );

        /**
         * Stops global note showing, and informs other observers
         * ands removes observer for global note
         * @param aObserver Observer for global note
         */
        void CancelNoteShowing( MIMPSReconnectNoteObserver* aObserver );

        /**
        * Mark the testing class a friend, because this class is not "public" class
        */
        friend class T_CIMPSReconnectNoteHandler; // CSI: 36 # Only used for testing

    protected:  // From CActive

        /**
        * @see CActive.
        */
        void DoCancel();

        /**
        * @see CActive.
        */
        void RunL();

        /**
        * @see CActive.
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CIMPSReconnectNoteHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TIMPSConnectionClient aClient );

    private: // helpers

        /**
        * Starts the call observing
        */
        void DoShowNoteL( MIMPSReconnectNoteObserver* aObserver );

        /**
        * Adds observer to observer array if not already there.
        * @param aObserver Call observer
        * @since 2.6
        */
        void AddObserverL( MIMPSReconnectNoteObserver* aObserver );

        /**
        * Removes observer to observer array if found from array
        * @param aObserver Call observer
        * @since 2.6
        */
        void RemoveObserver( MIMPSReconnectNoteObserver* aObserver );

        /**
        * Calls HandleNoteAnswered for all observers
        * @param TInt value of answer
        */
        void HandleNoteAnswered( TInt aAnswer );

        /**
        * Calls HandleNoteCanceled for all observers
        */
        void HandleNoteCancelled();

        /**
         * Loads prompt text resource
         * @return prompt text
         */
        HBufC* PromptTextLC();

        /**
         * Reads TBUF resource with given id
         * @param aNoteTextResourceId TBUF resource id
         * @return read resource as string
         */
        HBufC* ReadResourceLC( TInt aNoteTextResourceId );

        /**
         * Reads TBUF resource wit given id and replaces
         * %s with given text
         * @param aNoteTextResourceId TBUF resource id
         * @param aInsertText text to replace
         * @return read resource as string
         */
        HBufC* ReadResourceWithDynTextLC( TInt aNoteTextResourceId,
                                          const TDesC& aInsertText );

    private:    // Data

        /// Array for observing objects.
        RPointerArray<MIMPSReconnectNoteObserver> iObservers;

        /// Global confirmation query, owned
        CAknGlobalConfirmationQuery* iGlobalConfirmationQuery;

        /// A pointer to the connection UI, owned
        /// Used for fetching default server name
        CIMPSPresenceAAConnection* iConnectionUI;

        /// File server session, owned
        RFs                     iFs;

        /// IMPS AO plugin resource file, owned
        RResourceFile           iResFile;

        TIMPSConnectionClient   iClient;

    };

#endif      // __CIMPSRECONNECTNOTEHANDLER_H

// End of File

