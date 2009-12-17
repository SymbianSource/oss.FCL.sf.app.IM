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
* Description:  Holder for attribute decoding.
*
*/


#ifndef TDECODEATTRPARAMS_H
#define TDECODEATTRPARAMS_H

//  INCLUDES
#include "SServerPrefers.h"
#include <badesca.h>    // CDesCArray
#include "TCARequestQueue.h"

// FORWARD DECLARATIONS
class MPEngPresenceAttrModel2;
class MCAStoredContacts;
class MCAWatcherObserver;
class CCAState;

// CLASS DECLARATION

/**
 *  Data holder for presence attribute decoding
 *  @since 2.1
 */
struct TDecodeAttrParams
    {
    RPointerArray< CCAState >* iPresenceStates;
    RPointerArray< MPEngPresenceAttrModel2 > iArray;
    MCAStoredContacts* iContactStorage;
    MCAWatcherObserver* iWatcherObserver;
    CDesCArray* iStateOnline;
    CDesCArray* iStateOffline;
    CDesCArray* iStateUnknown;
    TInt iEntryIndex;
    SServerPrefers iServerPrefers;
    TCARequestQueue::TWaitCategory iWait;
    };

#endif      // TDECODEATTRPARAMS_H

// End of File

