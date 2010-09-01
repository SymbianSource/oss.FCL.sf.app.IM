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
* Description:  Sub session of the Presence server
*  				 It is used to maintain one session slot.
*
*/


#ifndef __RPENGSESSIONHOLDER_H__
#define __RPENGSESSIONHOLDER_H__

//  INCLUDES
#include <e32def.h>
#include "RPEngManagerClient.h"

// FORWARD DECLARATIONS
class RPEngManagerClient;
class TPEngWVCspServicesTree2;

// CLASS DECLARATION
/**
 *  Sub-session of the Presence Server
 *  It is used to maintain one session slot.
 *
 *  @lib PEngClient2
 *  @since 3.0
 */
class RPEngSessionHolder : public RSubSessionBase
    {

    public:		// Constructors and destructors

        /**
         * Destructor.
         */
        IMPORT_C  ~RPEngSessionHolder( void );

        /**
         * C++ default constructor.
         */
        IMPORT_C	RPEngSessionHolder( void );

    public:		// Connect close operations

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
                               const TDesC16& aAppId,
                               RPEngManagerClient& aManagerClient );
        /**
         *	Closes sub session to the Presence server
         *
         *	This can be used also for CleanUpStack support
         *	By default this will cause detach of the sub-session holder
         *	on the server side
         *
         *  @since 3.0
         */
        IMPORT_C void Close();

    public:		// New Opening methods

        /**
         *	Asynchronous request to open new network session
         *
         *
         *	LogIn fails if there already such a session active on
         *	the server side.
         *	Session is specified by the information passed at holder
         *	construction.
         *	If client still wants to get shared owner ship of the existing
         *	active session, Attach method has to be used instead
         *	Once LogIn is completed successfully this session holder
         *	becomes reference to the such a session and its
         *	lifetime is bound with it. How closing of this session
         *	holder affects session itself depends on the function which is
         *	used for closing of this handler. See:
         *	ForceLogOut(), Detach(), LeaveAlive()
         *
         *
         *	@since 3.0
         *	@param aLogInData buffer with the logIn data information
         *	@param aStatus - TRequestStatus, where server will
         *			signal completing of the LogIn request
         *	@return KErrNone if request went fine to server, otherwise
         *				system wide error code
         */
        IMPORT_C TInt LogIn( const TDesC8& aLogInData,
                             TRequestStatus& aStatus );

        /**
         *	Cancel LogIn request
         *	@since 3.0
         */
        IMPORT_C void CancelLogIn();


        /**
         *	Synchronous request to attach holder to existion session
         *
         *	Session holder is attached to the existing active session
         *	on the server.
         *	Session is specified by the information passed at holder
         *	construction.
         *
         *  @since 3.0
         *  @return KErrNone if request went fine to server, otherwise
         *				system wide error code
         */
        IMPORT_C TInt Attach();

        /**
         *	Synchronous request to attach holder to existion session
         *
         *	This will attach holder to the session which is active and
         *	was left to be active with specified process name.
         *	Once Holder is attached with the process name, process name
         *	is removed on the server side from the list of reserved
         *	process names which are reserving session to be opened
         *	even those processes are not alive.
         *	Session is specified by the information passed at holder
         *	construction.
         *
         *  @since 3.0
         *  @param aProcessName process name
         *  @return KErrNone if request went fine to server, otherwise
         *				system wide error code
         */
        IMPORT_C TInt Attach( const TDesC& aProcessName );

    public:	// New closing functions

        /**
         *	Asynchronous request to force network session close
         *
         *  Network session is closed on the server side for all
         *  application Ids
         *
         *	@since 3.0
         *	@param aStatus - TRequestStatus, where server will
         *			signal completing of the request
         *	@return KErrNone if request went fine to server, otherwise
         *				system wide error code
         */
        IMPORT_C TInt ForceLogOut( TRequestStatus& aStatus );

        /**
         *	Cancel LogOut request
         *
         *	@since 3.0
         */
        IMPORT_C void CancelLogOut();

        /**
         *	Detach Session holder
         *
         *	This function detaches session holder from active session
         *	on the server, it can result in closing of the session if
         *	this holder was last one using the session, therefore
         *	This function is asynchronous, though in most cases it
         *	will be completed immediately.
         *
         *  @since 3.0
         *	@param aStatus - TRequestStatus, where server will
         *			signal completing of the request
         *	@return KErrNone if request went fine to server, otherwise
         *				system wide error code
         */
        IMPORT_C TInt Detach( TRequestStatus& aStatus );

        /**
         *	Cancel Detach request
         *
         *	@since 3.0
         */
        IMPORT_C void CancelDetach();

        /**
         *	Leave session alive closing call
         *
         *	This call will leave session opened with reserved session
         *	for passed Process Name, untill this process name is not
         *	attached again some holder, session will stay active
         *	It should be used with caution, since it can leave to
         *	opened session without any client.
         *	How ever id client wants to disconnect from the session
         *	but keep session alive for next connection, this it the
         *	way to get job done.
         *
         *  @since 3.0
         *  @param aProcessName Process name to reserve session for
         *	@return KErrNone if request went fine to server, otherwise
         *				system wide error code
         */
        IMPORT_C TInt LeaveAlive( const TDesC& aProcessName );


        /**
         *  Session slot capabilities tree
         *
         *	retrieves Session slot's capability tree
         *	Function
         *
         *  @since 3.0
         *  @param aService tree to be filled with values of the session
         *			slot.
         *	@return KErrNone if session is opened and request to
         *		  server was successful, otherwise system wide error code
         */
        IMPORT_C TInt GetServiceTree(
            TPEngWVCspServicesTree2& aServiceTree );

        /**
         *  Session slot Log-In data
         *
         *	Returns log in data which were used for session slot opening
         *
         *  @since 3.0
         *  @param aLogInData buffer which will be on the return filled
         *			with log in data.
         *	@return KErrNone if session is opened and request to
         *		  server was successful, otherwise system wide error code
         */
        IMPORT_C TInt LogInData( HBufC8*& aLogInData );

    private:	// Data

        /// OWN: Flag is client is connected or not
        TBool						iConnected;

        /// OWN: Size of the Log in data buffer
        TInt						iLogInData;
    };

#endif      //  __RPENGSESSIONHOLDER_H__





















