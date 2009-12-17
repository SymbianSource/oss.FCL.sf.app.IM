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
* Description:  Description
*
*/

#ifndef __MIMPSPRTPLUGINCONNNECTIONMANAGER_H__
#define __MIMPSPRTPLUGINCONNNECTIONMANAGER_H__


#include "PImpsAdapWVCspVersion.h"
#include "CPresenceNotificationHandler.h"
// FORWARD DECLARATIONS
class MXIMPProtocolConnectionHost;
class CImpsPluginAccessHandler;
class CImpsPluginPureDataHandler;
class CPEngXMLParser;
class CPEngXmlSerializer;

/**
 * MImpsPrtPluginConnectionManager
 *
 * Imps Protocol Plugin Connection Manager's Interface
 *
 * @lib ?library
 * @since S60 v4.0
 */
class MImpsPrtPluginConnectionManager
    {

    public:

        /**
         * ?description
         *
         * @since S60 ?S60_version
         * @param ?arg1 ?description
         * @param ?arg2 ?description
         * @return ?description
         */

        virtual TInt32 IapId() = 0;

        virtual TDesC& ServerAddress() = 0;

        virtual TDesC& UserName() = 0;

        virtual TDesC& Password() = 0;

        virtual MXIMPProtocolConnectionHost& HandleToHost() = 0;

        virtual CImpsPluginAccessHandler& AccessHandler() = 0;

        virtual void ReserveSpaceForOneMoreRequestL() = 0;

        virtual void AddRequest( CActive* aRequestObject ) = 0;

        virtual void Remove( CActive* aRequestObject ) = 0;

        virtual CImpsPluginPureDataHandler& DataHandler() = 0;

        virtual void ManufactureDataHandlerL() = 0;

        virtual TBool IsDataHandlerCreatedL() = 0;

        virtual TImpsAdapWVCspVersion GetVersion() = 0;

        virtual void SetVersion( TImpsAdapWVCspVersion aVersion ) = 0;

        virtual RArray<HBufC>* PresenceAuthGrantReqList() = 0;

        virtual CPresenceNotificationHandler* PresNotifier() = 0;

    protected:

        /**
         * Protected destructor.
         * Instancies can't be destroyed via this interface.
         */
        virtual ~MImpsPrtPluginConnectionManager() {}

    };


#endif // __MIMPSPRTPLUGINCONNNECTIONMANAGER_H__


