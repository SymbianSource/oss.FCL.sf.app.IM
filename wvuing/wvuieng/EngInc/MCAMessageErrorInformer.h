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
* Description:  Inform error observers.
*
*/


#ifndef __MCAMESSAGEERRORINFORMER_H__
#define __MCAMESSAGEERRORINFORMER_H__

//	FORWARD CLASS DECLERATIONS
class MCAMessageErrorObserver;

// CLASS DECLARATION

/**
 *  Message error informer
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessageErrorInformer
    {
    public: // Interface

        /**
         * Register observer
         * @param aObserver, Observer to register
         */
        virtual void RegisterObserver( MCAMessageErrorObserver* aObserver ) = 0;

        /**
         * Unregister observer
         * @param aObserver, Observer to unregister
         */
        virtual void UnregisterObserver( MCAMessageErrorObserver* aObserver ) = 0;

    protected:
        /**
         * Virtual destructor
         */
        virtual ~MCAMessageErrorInformer() {}
    };

#endif      // __MCAMESSAGEERRORINFORMER_H__

// End of File
