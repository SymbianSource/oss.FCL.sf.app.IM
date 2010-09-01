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


#ifndef __CPUBLISHOWNPRESENCEREQUEST_H__
#define __CPUBLISHOWNPRESENCEREQUEST_H__


//  INCLUDES
#include <e32base.h>
#include <ximpbase.h>
#include "impsdebugprint.h"


// FORWARD DECLARATIONS
class MImpsPrtPluginConnectionManager;
class MPEngXMLParser;
class MPEngXMLSerializer;
class MPresenceInfo;

/**
 * CPublishOwnPresencerequest
 *
 * Issue Subscribe Presentity Group List Request to Network Server
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CPublishOwnPresencerequest: public CActive


    {

    public:

        static CPublishOwnPresencerequest* NewL(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        static CPublishOwnPresencerequest* NewLC(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        virtual ~CPublishOwnPresencerequest();

    private:

        CPublishOwnPresencerequest(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId  );
        void ConstructL();

    private: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    public:

        void PublishOwnPresenceL( const MPresenceInfo& aPresence );

        void ExtractPersonPresenceAttributesL(
            const MPresenceInfo& aPresence,
            MPEngXMLSerializer* aSerializer );

        void ExtractServicePresenceAttributesL(
            const MPresenceInfo& aPresence,
            MPEngXMLSerializer* aSerializer );

        void ExtractDevicePresenceAttributesL(
            const MPresenceInfo& aPresence,
            MPEngXMLSerializer* aSerializer );



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


#endif // __CPUBLISHOWNPRESENCEREQUEST_H__

