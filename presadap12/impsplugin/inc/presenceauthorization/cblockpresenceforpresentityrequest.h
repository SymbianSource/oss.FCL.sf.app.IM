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


#ifndef __CBLOCKPRESENCEFORPRESENTITYREQUEST_H__
#define __CBLOCKPRESENCEFORPRESENTITYREQUEST_H__


//  INCLUDES
#include <e32base.h>
#include <ximpbase.h>
#include "impsdebugprint.h"


// FORWARD DECLARATIONS
class MImpsPrtPluginConnectionManager;
class MPEngXMLParser;
class MPEngXmlSerializer;
class MXIMPIdentity;

/**
 * CBlockPresenceForPresentityRequest
 *
 * Issue Add Presentity Group Member Request to Network Server
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CBlockPresenceForPresentityRequest: public CActive


    {

    public:

        static CBlockPresenceForPresentityRequest* NewL(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        static CBlockPresenceForPresentityRequest* NewLC(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        virtual ~CBlockPresenceForPresentityRequest();

    private:

        CBlockPresenceForPresentityRequest(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId  );
        void ConstructL();

    private: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    public:

        void BlockPresenceForPresentityL(
            const MXIMPIdentity& aPresentityId );


    private: // data

        /**
         * Request Id from PrFw
         */

        TXIMPRequestId iRequestId;

        /**
         * Send data request to pure data handler generates the id
         */
        TInt iSendId;
        HBufC8* iResponse;

        /**
         * group display name
         * Not own.    *** Write "Not own" if some other class owns this object.
         * Ownership is transferred to CImpsPluginConnectionManager
         */

        HBufC16* iGroupName;

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


#endif // __CBLOCKPRESENCEFORPRESENTITYREQUEST_H__

