/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  View class for main-view
*
*/



#ifndef CCAREFRESHVIEW_H
#define CCAREFRESHVIEW_H

// INCLUDES
#include "CCAView.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CCAEmptyControl;
class CIdle;

// CLASS DECLARATION

/**
*  Main view for chat application
*
*  @lib chat.app
*  @since 1.2
*/
class CCARefreshView : public CCAView
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CCARefreshView();

        /**
         * Constructor
         */
        CCARefreshView( TInt& aRefreshFlag );

    private: // from base classes

        /**
        * From CAknView, Gets called from framework when activating this view
        * @param aPrevViewId Previous view id
        * @param aCustomMessageId Custom message's id
        * @param aCustomMessage Custom message
        * @see CAknView::DoActivateL
        */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView, Gets called from framework when deactivating this view
        * @see CAknView::DoDeactivate
        */
        void DoDeactivate();


    private: // new methods

        /**
         * callback for CIdle
         */
        static TInt Callback( TAny* aInstance );

    private: // Data

        // owns
        CCAEmptyControl* iContainer;

        /// Flag which is changed for telling if refresh is activated or not.
        TInt& iRefreshFlag;

    };

#endif // CCAREFRESHVIEW_H 

// End of File
