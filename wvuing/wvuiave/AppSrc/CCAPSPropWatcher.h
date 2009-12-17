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
* Description:  Publish & Subscribe property watcher
*
*/



#ifndef CCAPSPROPWATCHER_H
#define CCAPSPROPWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>


// CLASS DECLARATION
class MCAPSPropChangeObserver;
/**
*  Publish & Sbuscribe property watcher
*  The watcher is an active object
*
*  @lib chatng.app
*  @since Series 60 Platform 2.6
*/
class CCAPSPropWatcher : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAPSPropWatcher* NewL( MCAPSPropChangeObserver* aObserver,
                                       TUid aCategory,
                                       TUint aKey );

        /**
        * Destructor.
        */
        virtual ~CCAPSPropWatcher();

    public: // From CActive

        void RunL();
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CCAPSPropWatcher( MCAPSPropChangeObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TUid aCategory, TUint aKey );

    private:

        RProperty iProperty;
        MCAPSPropChangeObserver* iObserver;
    };

#endif      // CCAPSPROPWATCHER_H  

// End of File
