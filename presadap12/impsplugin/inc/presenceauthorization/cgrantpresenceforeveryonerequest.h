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


#ifndef __CGRANTPRESENCEFOREVERYONEREQUEST_H__
#define __CGRANTPRESENCEFOREVERYONEREQUEST_H__


//  INCLUDES
#include <e32base.h>
#include <ximpbase.h>
#include "impsdebugprint.h"


// FORWARD DECLARATIONS
class MImpsPrtPluginConnectionManager;
class MPEngXMLParser;
class MPEngXmlSerializer;
class MXIMPIdentity;
class MPresenceInfoFilter;

/**
 * CGrantPresenceForEveryoneRequest
 *
 * Issue Add Presentity Group Member Request to Network Server
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CGrantPresenceForEveryoneRequest: public CActive


    {

    public:

        static CGrantPresenceForEveryoneRequest* NewL(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        static CGrantPresenceForEveryoneRequest* NewLC(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        virtual ~CGrantPresenceForEveryoneRequest();

    private:

        CGrantPresenceForEveryoneRequest(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId  );
        void ConstructL();

    private: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    public:

        void GrantPresenceForEveryoneL(
            const MPresenceInfoFilter& aPif );


    private: // data

        /**
         * Request Id from PrFw
         */

        TXIMPRequestId iRequestId;

        /**
         * Send data request to pure data handler generates the id
         */
        TInt iSendId;

        /**
         * Response, to get it from pure data handler
         */
        HBufC8* iResponse;

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


#endif // __CGRANTPRESENCEFOREVERYONEREQUEST_H__

