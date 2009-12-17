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
* Description:  Connection Manager for IMPS Protocol adaptation
*
*/

#ifndef __CIMPSPLUGINCONNECTIONMANAGER_H__
#define __CIMPSPLUGINCONNECTIONMANAGER_H__

//  INCLUDES
#include <e32base.h>
#include <impsclient.h>
#include <impsaccesscli.h>
#include <impspurecli.h>
#include <flogger.h>
#include <ximpstatus.h>

#include "PEngPresenceEngineConsts2.h"
#include "MPEngSessionStatusObserver.h"
#include "impsconfig.h"
#include "impsconnectionmanagercontrolif.h"
#include "PImpsAdapWVCspVersion.h"
#include "CPresenceNotificationHandler.h"


// FORWARD DECLARATIONS

class MXIMPServiceInfo;
class MImpsConnectionObserver;
class CImpsPluginAccessHandler;
class CImpsPluginPureDataHandler;
class MXIMPProtocolConnectionHost;
class CImpsPluginLoginRequest;
class CImpsPluginLogoutRequest;
class CActive;



/**
 * CImpsPluginConnectionManager
 *
 * Every Imps Connection has Connection Manager Object.
 *
 * @lib ?library
 * @since S60 v4.0
 */
NONSHARABLE_CLASS( CImpsPluginConnectionManager ) : public CBase,
        public MImpsPrtPluginConnectionManager,
        public MPEngSessionStatusObserver
        // implement session status observer callback
    {

public:

    static CImpsPluginConnectionManager* NewL();
    virtual ~CImpsPluginConnectionManager();


private:

    CImpsPluginConnectionManager();
    void ConstructL();


public: // MImpsPrtPluginConnectionManager

    TInt32 IapId();
    TDesC& ServerAddress();
    TDesC& UserName();
    TDesC& Password();

    CImpsPluginAccessHandler& AccessHandler();
    CImpsPluginPureDataHandler& DataHandler();

    MXIMPProtocolConnectionHost& HandleToHost();
    void InitializeStatusL();

public:

    void StatusChangedL( TPEngNWSessionSlotState aNewState,
                         TPEngNWSessionSlotEvent aNewEvent );

public:

    void ReserveSpaceForOneMoreRequestL();
    void AddRequest( CActive* aRequestObject );
    void Remove( CActive* aRequestObject );
    CPEngXMLParser& Parser();
    void ManufactureDataHandlerL();
    TBool IsDataHandlerCreatedL();
    TImpsAdapWVCspVersion GetVersion();
    void SetVersion( TImpsAdapWVCspVersion aVersion );

public: // new methods


    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void SetHost( MXIMPProtocolConnectionHost& aHost );


    void SetConnectionParamsL( const TDesC& aServerAddress,
                               const TDesC& aUserName,
                               const TDesC& aPassword,
                               const TInt32 aIapId );

    RArray< HBufC >* PresenceAuthGrantReqList();

private: // data

    HBufC* iServerAddress;
    HBufC* iUserName;
    HBufC* iPassword;
    TInt32 iIapId;
    TBool iIsDataHandlerReady;

    CImpsPluginLoginRequest* iLoginRequest;
    CImpsPluginLogoutRequest* iLogoutRequest;

    RImpsEng iImpsClient;

    CImpsPluginAccessHandler* iAccessHandler;
    CImpsPluginPureDataHandler* iPureHandler;

    MXIMPStatus* iDisconnectStatusObj;
    MXIMPProtocolConnectionHost* iHostHandle;

    RPointerArray< CActive > iRequestArray;
    TImpsAdapWVCspVersion iCSPVersion;

public:  //data

    RArray< HBufC > iPresenceAuthGrantReqList;
    CPresenceNotificationHandler* iNotifier;
    CPresenceNotificationHandler* PresNotifier();
    };


#endif // __CIMPSPLUGINCONNECTIONMANAGER_H__
