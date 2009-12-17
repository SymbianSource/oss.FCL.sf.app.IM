/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
* A manager class for connection component.
*
*/


#ifndef CImpsConnManager_H
#define CImpsConnManager_H

//  INCLUDES
#include <e32base.h>
#include <msgconnmanagerapi.h>
#include <msgbearerobscallback.h>


// FORWARD DECLARATIONS
class CImpsSendReceive2;
class MMsgConnManager;

// CLASS DECLARATION

class CImpsConnManager : public CActive, public MMsgBearerObsCallback
    {
    public: // new methods

        /**
        * Two phase constructor
        * @param aSender CImpsSendReceive2 entity
        * @param aPriority CActive priority
        * @return CImpsConnManager entity
        */
        static CImpsConnManager* NewL( CImpsSendReceive2& aSender, TInt aPriority );

        /**
        * Destructor
        */
        virtual ~CImpsConnManager();

        /**
        * Start to wait open.
        * Use CActive::Cancel() to cancel the request.
        * @param aAIP IAP number in CommDb
        */
        void OpenAPL(
            TInt  aIAP, TBool aDelayed );

        /**
        * Close AP
        * @param aTotalClose ETrue if bearer statu events not wanted,
        *        EFalse if PDP context released only.
        * @return error code
        */
        void CloseAP( TBool aTotalClose );

        /**
        * Deletes members while active scheduler is still running
        */
        void Destroy();

        /**
        * ConnectionManager accessor
        * Leaves if handle not created.
        * @return Connection Manager entity
        */
        MMsgConnManager& ManagerHandleL() const;

        /**
        * Start a delayed request
        * @return error code KErrNotReady if previous request incomplete
        */
        TInt StartDelayed();

    public: // from base classes

        /**
        * Callback from base class
        * @param aBearerEvent The event that took place
        * @param aAuthoritativeClose indicates whether connection
        *              was authoritatively closed (i.e. by the user)
        */
        void HandleBearerEventL( TBool aAuthoritativeClose,
                                 TMsgBearerEvent aBearerEvent );


    protected:
        // From base class
        void DoCancel();
        void RunL();

    private:

        /**
        * By default constructor is private.
        */
        void ConstructL(  );

        /**
        * Constructor
        * @param aServer WV engine server
        * @param aPriority Active objects priority
        */
        CImpsConnManager( CImpsSendReceive2& aSender, TInt aPriority );

        /**
        * Activate entity
        * @return error code KErrNotReady if previous request incomplete
        */
        TInt ActivateMe();


    protected:
        CImpsSendReceive2&      iSender;
        TImpsConnectionState    iState;
        TImpsTransCmd           iCmd;
        TMsgBearerEvent         iConnState;
        MMsgConnManager*        iManager;
        TInt                    iIAP;
        TBool                   iRunL;
        RTimer                  iTimer;
    };


#endif      // ?INCLUDE_H   

// End of File
