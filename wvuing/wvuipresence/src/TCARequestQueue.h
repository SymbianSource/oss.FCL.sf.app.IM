/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Request queue handler
*
*/



#ifndef TCAREQUESTQUEUE_H
#define TCAREQUESTQUEUE_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Request queue handler
*
*  @lib CAPresence.lib
*  @since 2.1
*/
class TCARequestQueue
    {
    public: // Enums
        enum TWaitCategory
            {
            EFetcherWait,
            EPublisherWait,
            EContactListUpdater,
            EAttributeListUpdater,
            EDecodeAttrWait,
            EAppendContactsWait,
            EReactiveAuthWait,
            ENoWaitNeeded    // This can be used to tell if wait is needed.
            };

    public: // New functions

        void WaitForResponseL( TWaitCategory aCategory );
        void ResponseReceived( TWaitCategory aCategory );

    private:
        CActiveSchedulerWait* Wait( TWaitCategory aCategory );

    private:    // Data
        CActiveSchedulerWait iFetcherWait;
        CActiveSchedulerWait iPublisherWait;
        CActiveSchedulerWait iContactListUpdaterWait;
        CActiveSchedulerWait iAttributeListUpdaterWait;
        CActiveSchedulerWait iDecodeAttrWait;
        CActiveSchedulerWait iAppendContactsWait;
        CActiveSchedulerWait iReactiveAuthWait;
    };

#endif      // TCAREQUESTQUEUE_H

// End of File
