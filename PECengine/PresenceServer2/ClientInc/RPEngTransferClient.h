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
* Description:   Sub-session of the Presence Server
*  It is used to update date of the PEC engine to network
*
*/


#ifndef __RPENGTRANSFERCLIENT_H__
#define __RPENGTRANSFERCLIENT_H__

//  INCLUDES
#include	<e32base.h>
#include	"PEngPresenceEngineConsts2.h"

// FORWARD DECLARATIONS
class RPEngManagerClient;

// CLASS DECLARATION
/**
 *  Sub-session of the Presence Server
 *  It is used to update data of the Presence engine to network
 *
 *  @lib PEngClient2
 *  @since 3.0
 */
class RPEngTransferClient : public RSubSessionBase
    {

    public:		// Constructors and destructors

        /**
         * Destructor.
         */
        IMPORT_C  ~RPEngTransferClient();

        /**
         * C++ default constructor.
         */
        IMPORT_C	RPEngTransferClient();

    public:		// New methods

        /**
         *	Connect this sub-session to the Presence Server
         *
         *	Client is connected to the server with defined session Id
         *	and application Id
         *
         *  @since 3.0
         *  @param aSessionId packed session slot Id
         *	@param aAppId application Id
         *	@param aManagerClient main session client
         *  @return KErrNone if connected or system wide error code
         */
        IMPORT_C TInt Connect(	const TDesC8& aSessSlotId,
                               const TDesC& aAppId,
                               RPEngManagerClient& aManagerClient );

        /**
         *	Closes sub session to the Presence server
         *
         *  Also all asynchronous requests are canceled on the server side
         *  when this sub-session is cleaned
         *
         *  @since 3.0
         */
        IMPORT_C void Close();

    public:		// New operation of the client


        /**
         *	Update data to the presence Network
         *
         *  Passed TRequestStatus can be completed with following codes:
         *     Positive complete code:
         *	     	 Update operation went fine and there is result buffer
         *           on the server side waiting to be fetched, while
         *           completed number is size of the buffer need on
         *           the client side.
         *     Zero:
         * 			 Update operation went fine, but there is no detailed
         *		     result buffer waiting to be fetched on the server side
         *     Negative complete code:
         *           During update operation occured system wide error
         *           which same as complete code. Update operation was not
         *           completed successfuly.
         *
         *  @since 3.0
         *  @param aOperation, TPEngTransactionOperation enumeration
         *  @param aData detailed data if required by the update operation
         *  @return KErrNone if message went to the server
         *			KErrDisconnected if client is not connected
         */
        IMPORT_C TInt UpdateData(	TInt aOperation,
                                  const TDesC& aData,
                                  TRequestStatus& aStatus );

        /**
         *	Fetch result for last Update data call
         *
         *  @since 3.0
         *  @param aRequestResult descriptor buffer on the client side, when
         *         will be put detailed result buffer of the last request
         *  @return KErrNone if request was canceled
         *			KErrDisconnected if client is not connected
         *			or system wide error code if something went wrong
         */
        IMPORT_C TInt FetchLastRequestResult( TDes8& aRequestResult );

        /**
         *	Cancel last Asynchronous request update data
         *
         *  @since 3.0
         */
        IMPORT_C void CancelUpdateData();

    private:	// Data

        // Flag is client is connected or not
        TBool					iConnected;
    };

#endif      //  __RPENGTRANSFERCLIENT_H__





















