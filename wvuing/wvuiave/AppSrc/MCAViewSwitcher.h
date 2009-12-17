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
* Description:  General interface for switching chat-client views
*
*/


#ifndef MCAVIEWSWITCHER_H
#define MCAVIEWSWITCHER_H

#include <e32cmn.h>
#include "CAExternalInterface.h"
//FORWARD DECLARATIONS
class MCAStoredContact;
class CCAStatusPaneHandler;
class CEikButtonGroupContainer;


// CLASS DECLARATION

/**
*  General interface for switching chat-client views
*
*  @lib chat.app
*  @since 1.2
*/
class MCAViewSwitcher
    {

    public: // New functions

        /**
        * Activates the given view.
        * @param TUid Id of the view to be activated
        */
        virtual void SwitchViewL( TUid aViewId ) = 0;

        /**
        * Activates the last view or given view if there is none.
        * @param TUid Id of the view to be activated
        */
        virtual void SwitchViewBackL( TUid aViewId, TUid aCustomMessageId = KUidMainView,
                                      const TDesC8& aCustomMessage = KNullDesC8 ) = 0;

        /**
         * Switches view to given viewid
         * @param aViewId View id
         * @param aCustomMessageId Custom message id
         * @param aCustomMessage Custom message
         */
        virtual void SwitchViewL( TUid aViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage ) = 0;

        /**
         * Register info to switch back to given view
         * @param aViewId View id to register for back-function
         * @param aCustomMessageId Custom message id
         * @param aCustomMessage pointer to Custom message,
         *                       ownership is transfered
         * @param aNextView The view which is activated next
         */
        virtual void RegisterSwitchBack( TUid aViewId,
                                         TUid aCustomMessageId,
                                         HBufC8* aCustomMessage,
                                         TUid aNextView ) = 0;

        /**
         * Deregisters switch back info.
         * @since S60 v3.1
         * @param aViewId Id of view to be deregistered
         * @param aTarget Identification of view, wv id
         *                if view is conversation view,
         *                group id if view is chat view.
         */
        virtual void DeRegisterSwitchBack( TUid aViewId,
                                           const TDesC& aTarget ) = 0;

        /**
         * Returns handle to statuspane
         * @return Statuspane instance
         */
        virtual CCAStatusPaneHandler* CAStatusPane() const = 0;

        /**
         * Checks if view exists.
         * @param aViewId View id
         * @return ETrue, if exists, EFalse if not.
         */
        virtual TBool ViewExists( TUid aViewId ) = 0;

        /**
         * @return Active view's id
         */
        virtual TUid ActiveViewId() = 0;

    protected:

        /**
         * Destructor
         */
        virtual ~MCAViewSwitcher() {};

    };

#endif      // MCAVIEWSWITCHER_H

// End of File
