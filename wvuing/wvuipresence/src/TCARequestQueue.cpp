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



// INCLUDE FILES
#include    "TCARequestQueue.h"
#include    "ChatDebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TCARequestQueue::WaitForResponse
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TCARequestQueue::WaitForResponseL( TWaitCategory aCategory )
    {
    CHAT_DP_TXT( "TCARequestQueue, wait requested" );
    CActiveSchedulerWait* wait = Wait( aCategory );

    if ( wait )
        {
        if ( wait->IsStarted() )
            {
            CHAT_DP_TXT(
                "TCARequestQueue, given category wait is already active!" );
            // Server can handle only 1 certain request per time
            User::Leave( KErrServerBusy );
            }

        wait->Start();
        }
    else
        {
        // If given category was not supported, then leave
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// TCARequestQueue::ResponseReceived
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TCARequestQueue::ResponseReceived( TWaitCategory aCategory )
    {
    CHAT_DP_TXT( "TCARequestQueue, wait stop requested" );

    CActiveSchedulerWait* wait = Wait( aCategory );
    if ( wait )
        {
        if ( wait->IsStarted() )
            {
            wait->AsyncStop();
            }
        }
    }

CActiveSchedulerWait* TCARequestQueue::Wait( TWaitCategory aCategory )
    {
    switch ( aCategory )
        {
        case EFetcherWait:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation \
             to EFetcherWait" );
            return &iFetcherWait; // Does not need break because return
            }
        case EPublisherWait:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation to \
             EPublisherWait" );
            return &iPublisherWait; // Does not need break because return
            }
        case EContactListUpdater:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation to \
             EContactListUpdater" );

            // Does not need break because return
            return &iContactListUpdaterWait;
            }
        case EAttributeListUpdater:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation to \
             EAttributeListUpdater" );
            // Does not need break because return
            return &iAttributeListUpdaterWait;
            }
        case EDecodeAttrWait:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation to \
                      EDecodeAttrWait" );
            return &iDecodeAttrWait; // Does not need break because return
            }
        case EAppendContactsWait:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation to \
                      EAppendContactsWait" );
            return &iAppendContactsWait; // Does not need break because return
            }
        case EReactiveAuthWait:
            {
            CHAT_DP_TXT( "TCARequestQueue mapping wait operation to \
                      EReactiveAuthWait" );
            return &iReactiveAuthWait; // Does not need break because return
            }
        default:
            {
            CHAT_DP_TXT( "TCARequestQueue UNSUPPORTED wait category!!!" );
            return NULL; // Does not need break because return
            }
        }
    }

//  End of File
