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
* Description:  IMPS Connection UI Agent.
*
*/

#ifndef __CIMPSCONNECTIONUINGAGENT_H
#define __CIMPSCONNECTIONUINGAGENT_H

// INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>

// FORWARD DECLARATION
class CIMPSConnectionUiAgentImp;


// CLASS DECLARATION
/**
 * CIMPSConnectionUiAgent.
 *
 * Connection UI agent performs notification and query
 * services for the Connection UI's behalf on the client
 * application context. Client application should have
 * just one CIMPSConnectionUiAgent instance in its execution
 * context. Usually this is owned directly by the AppUI
 * or similar singleton instance.
 *
 * Client may itself deside which services to start and when.
 * However, the CIMPSConnectionUiAgent implementation is
 * designed so that the client may start all needed services
 * directly in its construction, and leave them so, without
 * any further actions. CIMPSConnectionUiAgent itself
 * follows the registered client group online status,
 * and acts based on that. Notification and query services
 * may be stopped however if the user navigates in the host
 * application to certain state where UI interrupts are not
 * allowed.
 *
 * @lib IMPSConnectionUi
 * @since 2.1
 */
class CIMPSConnectionUiAgent : public CBase
    {
    public:  // Two-phased constructor and destructor

        /**
         * Two-phased constructor.
         * @param aClient The client for which handle
         * IMPS connection related UI notifications etc.
         */
        IMPORT_C static CIMPSConnectionUiAgent* NewL( TIMPSConnectionClient aClient );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSConnectionUiAgent();



    public:  // New service handling methods


        /**
         * Starts delivering IMPS connection related UI notifications
         * and queries in client process UI.
         *
         * It is up to client to decide which services
         * to start. Available services are listed in
         * TIMPSConnectionAgentUiService enumeration.
         *
         * Starting a service fails with leave if:
         * - if running out of resourcies (KErrNoMemory)
         * - or with other standard Symbian OS error codes
         *   coming from underlying implementation.
         *
         * @since 2.1
         * @see TIMPSConnectionAgentUiService
         * @param aService The service to start.
         * @return Service start status - always KErrNone.
         */
        IMPORT_C TInt StartServiceL( TIMPSConnectionUiAgentService aService );


        /**
         * Stops defined service.
         *
         * @since 2.1
         * @param aService The service to stop.
         * @return Service stop status - always KErrNone.
         */
        IMPORT_C TInt StopService( TIMPSConnectionUiAgentService aService );


        /**
         * Stops all previously started services.
         *
         * @since 2.1
         */
        IMPORT_C void StopAllServices();


        /**
         * Sets the new resource file to use in Connection UI.
         *
         * @param aNewResourceFile the new resourcefile to be used
         * @since 2.6
         */
        IMPORT_C void UseResourceFileL( const TDesC& aNewResourceFile );

    private: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSConnectionUiAgent();


        /**
         * Symbian OS constructor.
         */
        void ConstructL( TIMPSConnectionClient aClient );


    private: //Data
        CIMPSConnectionUiAgentImp*        iImp;          ///<Implementation, owned
        TAny*                             iReserved1;    ///<Reserved, owned
    };



#endif      //__CIMPSCONNECTIONUINGAGENT_H

//  End of File

