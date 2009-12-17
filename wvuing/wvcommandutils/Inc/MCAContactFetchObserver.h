/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer interface to handle contact fetching.
*
*/


#ifndef MCACONTACTFETCHOBSERVER_H__
#define MCACONTACTFETCHOBSERVER_H__

//  INCLUDES
#include <e32def.h>

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Observer interface to handle contact fetching
 *
 *  @lib Chat.exe
 *  @since S60 v3.1
 */
class MCAContactFetchObserver
    {

    public: // Interface

        /**
         * Handle complete event.
         * @param aError Result of fetching process.
         */
        virtual void HandleFetchCompleteL( TInt aError ) = 0;

    protected: // For protection.

        /**
         * Destructor for protection
         */
        virtual ~MCAContactFetchObserver() {}
    };

#endif      // MCACONTACTFETCHOBSERVER_H__

// End of File
