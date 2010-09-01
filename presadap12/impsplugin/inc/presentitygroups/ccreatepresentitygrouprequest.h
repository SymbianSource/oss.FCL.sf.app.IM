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
* Description:  Create Presentity Group Request
*
*/


#ifndef __CIMPSPLUGINCREATEPRESENTITYGROUPREQUEST_H__
#define __CIMPSPLUGINCREATEPRESENTITYGROUPREQUEST_H__


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
 * CCreatePresentityGroupRequest
 *
 * Issue Request to Network Server to Create Contact List
 *
 * @lib ?library
 * @since S60 v4.0
 */

class CCreatePresentityGroupRequest: public CActive


    {

    public:

        static CCreatePresentityGroupRequest* NewL(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        static CCreatePresentityGroupRequest* NewLC(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId );

        virtual ~CCreatePresentityGroupRequest();


    private:

        CCreatePresentityGroupRequest(
            MImpsPrtPluginConnectionManager& aConnMan,
            TXIMPRequestId aRequestId  );
        void ConstructL();


    private: // from CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );


    public:

        void CreatePresentityGroupRequestL( const MXIMPIdentity& aIdentity,
                                            const TDesC16& aDisplayName );


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
         * Response that is received from the server
         */
        HBufC8* iResponse;

        HBufC16* iGroupId;
        /**
         * Group Displayname for the created group
         */
        HBufC16* iGroupDisplayName;


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


#endif // __CIMPSPLUGINCREATEPRESENTITYGROUPREQUEST_H__

