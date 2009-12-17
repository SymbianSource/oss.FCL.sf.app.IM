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
* Description:  Settings view
*
*/



#ifndef CCASETTINGSVIEW_H
#define CCASETTINGSVIEW_H

//  INCLUDES
#include "CCAView.h"
#include "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class CCASettingsViewContainer;
class CIMPSSAPSettingsStore;

// CLASS DECLARATION

/**
*  View class for settings view
*
*  @lib chat.app
*  @since 1.2
*/
class CCASettingsView : public CCAView
    {
    public:  // Constructors and destructor

        /**
        * 2nd constructor
        * @param aAvkonViewResourceId View resource
        * @param aViewId View Id
        * @param aSAPSettingsStore SAP settings store
        */
        void ConstructL( TInt aAvkonViewResourceId,
                         TUid aViewId );


        /**
        * Destructor.
        */
        virtual ~CCASettingsView();


    private: // From CAknView

        /**
        * @see CAknView
        */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * @see CAknView
        */
        void DoDeactivate();

        /**
        * @see CAknView
        */
        void HandleCommandL( TInt aCommand );

        /**
        * @see CAknView
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: //Data

        TVwsViewId iPrevViewId; // Previous view Id

        // Owns. Container for view.
        CCASettingsViewContainer* iContainer;

#ifdef RD_SERVICE_ENHANCEMENTS
        TBool iServSettingsDialogWasActive;
#endif  // RD_SERVICE_ENHANCEMENTS
    };

#endif      // CCASETTINGSVIEW_H   

// End of File
