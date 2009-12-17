/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Intended to be implemented by the observer
*
*/



#ifndef __MPENGSIDCHANGEOBSERVER_H__
#define __MPENGSIDCHANGEOBSERVER_H__

//  INCLUDES

#include <e32def.h>
#include <badesca.h>

/**
 *  Storage Id change observer abstract call back interface
 *	This class is intended to be implemented by the storage id change
 *	observer.
 *	Changed of the storage Ids are notified through it.
 *
 *  @lib PEngStoreMan2.dll
 *  @since 3.0
 */

// CLASS DECLARATION
class MPEngSIDChangeObserver
    {
    public: // New functions

        /**
         *	Handle change of the Storage IDs
         *	Changed storage IDs are passed in the array,
         *	client can modify passed array as he wishes
         *
         *	@since 3.0
         *	@param aChangedSIDs array of the changed SIDs
         *				array can be modified by the client
         */
        virtual void HandleSIDsChangeL( CPtrCArray& aChangedSIDs ) = 0 ;

        /**
         *	Called when SIDs notifier fails
         *
         *	@since 3.0
         *	@param aError is the error code.
         */
        virtual void HandleSIDNotifyError( TInt aError ) = 0;

    protected:  //Destructor

        /**
         * Protected destructor.
         * Storage Id change observer can't be destroyed via this interface.
         */
        virtual ~MPEngSIDChangeObserver() {}

    };

#endif      // __MPENGSIDCHANGEOBSERVER_H__

// End of File
