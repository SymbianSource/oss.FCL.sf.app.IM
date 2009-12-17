/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: SAP settings observer test cases
*
*/


#ifndef __T_WVSAPSettingsObserverTestCases_H__
#define __T_WVSAPSettingsObserverTestCases_H__

// INCLUDES
#include "MIMPSSAPObserver.h"
#include "CObserverWait.h"

//  FORWARD DECLARATIONS
class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;



//  CLASS DEFINITION
class WVSAPSettingsObserverTestCases : public MIMPSSAPObserver

    {
    public:     // Constructors and destructors

        static WVSAPSettingsObserverTestCases* NewL();
        static WVSAPSettingsObserverTestCases* NewLC();
        ~WVSAPSettingsObserverTestCases();
        WVSAPSettingsObserverTestCases();
    private:    // New methods

        void ConstructL();

    public:    // Test case functions
        TInt TestNotify_AddSapL();
        TInt TestNotify_RemoveSapL();

        //TInt TestNotify_SameStoreGetOperationsL();

    public: //from MIMPSActiveDbObserver
        /**
         * Method called after db event.
         */
        void HandleSAPEvent( TSAPEvent aEvent );


    private:  // Helper functions.
        void WaitQuedEvents( TInt aEventCountStart );
        TInt CountQueuedEvents( TInt aEventCountStart );
        void ResetTheObsState();

    private:    // Data
        CIMPSSAPSettingsStore* iStore;

        CIMPSSAPSettings*      iSettingsEmpty;
        CIMPSSAPSettings*      iSettingsFilled;
        CIMPSSAPSettings*      iDefaultSAP;
        CIMPSSAPSettings*      iLoggedInSAP;

        TUint32              iDefaultUid;
        TUint32              iExistingUid1;
        TUint32              iExistingUid2;
        TUint                iSAPNameIndex;

        CObserverWait<TBool, TInt >  *iObsWait;
    };

#endif      //  __T_WVSAPSettingsObserverTestCases_H__

// end of file

