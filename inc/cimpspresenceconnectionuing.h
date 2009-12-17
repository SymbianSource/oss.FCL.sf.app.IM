/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Connection UI.
*
*/

#ifndef __CIMPSPRESENCECONNECTIONUING_H__
#define __CIMPSPRESENCECONNECTIONUING_H__

// INCLUDES
#include <E32base.h>
#include <PEngPresenceEngineConsts2.h>
#include <impspresenceconnectionuiconstsng.h>



// FORWARD DECLARATION
class MIMPSConnProcessObserver;
class CIMPSSAPSettingsStore;
class CIMPSPresenceConnectionUiImp;
class CIMPSSAPSettings;
class CPEngNWSessionSlotID2;

// CLASS DECLARATION
/**
 * CIMPSPresenceConnectionUi.
 *
 * CIMPSPresenceConnectionUi encapsulates remote Presence Server connection /
 * disconnection procedures. CIMPSPresenceConnectionUi allows applications
 * just execute the connection procedure, CIMPSPresenceConnectionUi shows
 * all required UI notes, asks login data from user when needed, etc.
 *
 * @lib IMPSConnectionUi
 * @since 3.0
 */
class CIMPSPresenceConnectionUi : public CBase
    {
    public:  // Two-phased constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @param aClient The client using the Connection UI
         * @param aSapStore The SAP settings store to use.
         * Given SAP settings store object lifetime must be
         * longer than created CIMPSPresenceConnectionUi.
         * If no SAP settings store provided, creates own
         * SAP Settings store internally when needed.
         */
        IMPORT_C static CIMPSPresenceConnectionUi* NewL(
            TIMPSConnectionClient aClient,
            CIMPSSAPSettingsStore* aSapStore = NULL );

        /**
         * Two-phased constructor.
         * Leaves CIMPSPresenceConnectionUi object on the
         * CleanupStack.
         *
         * @param aClient The client using the Connection UI
         * @param aSapStore The SAP settings store to use.
         * Given SAP settings store object lifetime must be
         * longer than created CIMPSPresenceConnectionUi.
         * If no SAP settings store provided, creates own
         * SAP Settings store internally when needed.
         */
        IMPORT_C static CIMPSPresenceConnectionUi* NewLC(
            TIMPSConnectionClient aClient,
            CIMPSSAPSettingsStore* aSapStore = NULL );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSPresenceConnectionUi();


    public:  // New connection handling methods


        /**
         * Connects to remote presence server.
         *
         * Handles error situations internally by showing
         * a proper error note. Asks login data and used server
         * from user when needed. Retries to open the connection if
         * needed.
         *
         * Connection status is returned as return value.
         * Internal Symbian OS errors (like running out of resources,
         * or other fatal errors coming from underlying Presence Engine)
         * are handled by leaving.
         *
         * The client must intialize an instance of CPEngNWSessionSlotID2
         * and give it as a parameter. When the call returns the slot ID
         * contains the current active connection's network session slot ID.
         *
         * @since 3.0
         * @param aClient The client using the Connection UI to login
         * to presence service. (This client is logged in.)
         * @param aLoginType Wanted Connection UI behaviour.
         * @param aNWSessionSlotID a network session slot ID, on return will contain
         *        the active network session slot ID
         * @param aObserver The observer to notify from login process
         * events.
         * @return Login operation status, one of following:
         * - KErrNone ==> Login succeeded
         * - KErrNotFound ==> Missing the whole default SAP / or no SAP's at all
         * - KErrArgument ==> Default / selected SAP to login was
         *                    missing some required fields. (URL & AccessPoint)
         * - KErrCancel ==> User has cancelled the login at some point.
         * - KErrAlreadyExist ==> Current client is already logged in.
         *                        (Thus not a real error.)
         * - KErrGeneral ==> General failure during login (no connection
         *                   to network, network access denied, etc..)
         */
        IMPORT_C TInt LoginL( TIMPSConnectionClient aClient,
                              TIMPSLoginType aLoginType,
                              CPEngNWSessionSlotID2& aNWSessionSlotID,
                              MIMPSConnProcessObserver* aObserver = NULL,
                              CIMPSSAPSettings* aSap = NULL, TBool aLastLoginSap = EFalse );


        /**
         * Disconnects from remote presence server.
         *
         * Internal Symbian OS errors (like running out of resources,
         * or other fatal errors coming from underlying Presence Engine)
         * are handled by leaving.
         *
         * @since 3.0
         * @param aNWSessionSlotID a network session slot ID to logout from.
         * @param aObserver The observer to notify from logout process
         * events.
         * @return Currently always KErrNone.
         */
        IMPORT_C TInt LogoutL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                               MIMPSConnProcessObserver* aObserver = NULL );


        /**
         * Handles the application exit according the
         * IMPS Server Settings specification.
         *
         * Internal Symbian OS errors (like running out of resources,
         * or other fatal errors coming from underlying Presence Engine)
         * are handled by leaving.
         *
         * @since 3.0
         * @param aClient Which client categorys exit to handle.
         *
         * @param TIMPSExitType Wished  behaviour of handle exit.
         * EIMPSNormalExit:
         * If there exist open connection for the given application
         * category, and the connection type is is  manual or application
         * launch, a confirmation query is opened wether to close
         * open connection or not. Connection is closed if desired so.
         * EIMPSForceExit:
         * No confirmation queries etc. isn't shown.
         * Existing open connection is silently closed.
         *
         * EIMPSLeaveSessionOpenExit:
         * The network session is left open by storing the network session
         * slot ownership to Presence engine. This ownership is restored
         * next time the application starts
         *
         * @param aNWSessionSlotID the current network session slot ID.
         * @param aObserver The observer to notify from logout process
         * events.
         * @return Currently always KErrNone.
         */
        IMPORT_C TInt HandleApplicationExitL( TIMPSConnectionClient aClient,
                                              TIMPSExitType aExitType,
                                              const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                              MIMPSConnProcessObserver* aObserver = NULL );

        /**
         * Gets the remote presence server connection status.
         *
         * @since 3.0
         * @param aClient The client which connection status
         * to check.
         * @return ETrue if client is logged in, else EFalse.
         */
        IMPORT_C TBool LoggedInL( TIMPSConnectionClient aClient );

        /**
         * Gets the currently active network session slot id.
         * If there is no active network session slot ID return
         * the default SAP's network session slot ID.
         * If there are no SAPs defined in the system leaves
         * with KErrNotFound.
         * Note that ownership is transferred to the caller.
         *
         * @since 3.0
         * @param aClient The client which slots to get
         * @return the network session slot ID
         */
        IMPORT_C CPEngNWSessionSlotID2* GetActiveNWSessionSlotIDL( TIMPSConnectionClient aClient );


        /**
         * Gets the remote presence server settings.
         *
         * Gets the remote presence server settings for
         * current connection. There can be just one
         * real network presence connection active at time,
         * and this method retrieves details of that
         * connection. (Different client groups share
         * logically on real network connection.)
         *
         * If there isn't a active (logged in) connection
         * for any of known clients, returns KErrNotFound.
         * Else the given aSap is filled with connection
         * settings and KErrNone is returned.
         *
         * If sap get fails due out of resources or to some
         * other fatal error coming from underlying PEC engine,
         * leaves with system error codes. In this case
         * aSap parameter state is undetermined.
         *
         * @since 3.0
         * @param aNWSessionSlotID the current active network session slot ID
         * @param aSap The SAP container to fill with current
         * connection settings.
         * @return KErrNone if there exist a connection for known
         * client. Else KErrNotFound.
         */
        IMPORT_C TInt GetLoggedInSapL( const CPEngNWSessionSlotID2& aNWSessionSlotID,
                                       CIMPSSAPSettings& aSap );



        /**
         * Gets the pure connection status from Presence Engine.
         *
         * @since 3.0
         * @param aNWSessionSlotID a network session slot ID
         * @return Connection status from Presence Engine.
         */
        IMPORT_C TPEngNWSessionSlotState PureServiceStatusL(
            const CPEngNWSessionSlotID2& aNWSessionSlotID );


        /**
         * Gets the currently ongoing connection
         * state change operation. Returned operation
         * is client wide, so it doesn't care about which
         * client is doing the operation.
         *
         * @since 3.0
         * @param aAddtionalDesc The descriptor describing / related
         * to current connection operation. Method allocates a
         * buffer for descriptive descriptor if needed and returns
         * its to caller trough the HBufC*& aDetailDesc.
         * Ownership of the created buffer is returned to
         * caller. (NOTE! Only EIMPSConnOppClientLogin returns
         * descriptive text, currently connected server name.
         * Other operations return NULL.)
         * @return The Current connection operation.
         */
        IMPORT_C TIMPSConnectionOperation CurrentConnectionOperationL( HBufC*& aAddtionalDesc );

    public:  // Other new functions

        /**
         * Displays a list query for selecting a domain ("@domain.com" etc.)
         * @param aSelectedDomain Here is copied the domain user selected.
         * @param aSap Current SAP settings, previously selected domain is
         *             stored per SAP.
         * @return If user cancelled the query return 0,
         *         otherwise the dialog dismiss key code
         * @since 3.1u
         */
        IMPORT_C TInt DisplayDomainSelectionQueryL( TDes& aSelectedDomain,
                                                    CIMPSSAPSettings* aSap );

    private: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSPresenceConnectionUi( TIMPSConnectionClient aClient );


        /**
         * Symbian OS constructor.
         */
        void ConstructL( CIMPSSAPSettingsStore* aSapStore );


    private: //Data
        CIMPSPresenceConnectionUiImp*     iImp;          ///<Implementation, owned
        TIMPSConnectionClient             iClient;       ///<Client using the Connection UI
        TAny*                             iReserved1;    ///<Reserved, owned
    };



#endif      //__CIMPSPRESENCECONNECTIONUING_H__

//  End of File

