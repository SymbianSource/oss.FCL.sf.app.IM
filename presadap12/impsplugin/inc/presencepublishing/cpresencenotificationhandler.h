/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Protocol implementation for Presence Framework
*
*/


#ifndef __CPresenceNotificationHandler_H__
#define __CPresenceNotificationHandler_H__


//  INCLUDES
#include <e32base.h>
#include <ximpbase.h>
#include "impsdebugprint.h"


// FORWARD DECLARATIONS
class MImpsPrtPluginConnectionManager;
class MPEngXMLParser;
class MPEngXmlSerializer;

/**
 * CPresenceNotificationHandler
 *
 * Issue Subscribe Presentity Group List Request to Network Server
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CPresenceNotificationHandler: public CActive


    {

    public:

        static CPresenceNotificationHandler* NewL(
            MImpsPrtPluginConnectionManager& aConnMan
        );

        static CPresenceNotificationHandler* NewLC(
            MImpsPrtPluginConnectionManager& aConnMan
        );

        virtual ~CPresenceNotificationHandler();

    private:

        CPresenceNotificationHandler( MImpsPrtPluginConnectionManager& aConnMan );
        void ConstructL();

    private: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    public:

        void StartListeningL();

        void HandleIncomingDataL();

        void ProcessIncomingDataL();

    private: // data

        /**
         * Send data request to pure data handler generates the id
         */
        TInt iSendId;
        HBufC8* iResponse;
        HBufC16* iListId;


        /**
         * ?description_of_pointer_member
         * Own.  *** Write "Own" if this class owns the object pointed to; in
                     other words, if this class is responsible for deleting it.
         */

        MPEngXMLParser* iParser;

        /**
         * Imps Protocol Plugin Connection Manager
         * Not own.  *** Write "Not own" if some other class owns this object.
         */
        MImpsPrtPluginConnectionManager& iConnMan;

    };


#endif // __CPresenceNotificationHandler_H__

