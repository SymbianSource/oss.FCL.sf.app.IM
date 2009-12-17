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
* Description:  ImIndicator plugin implementation
*
*/



#ifndef CIMINDICATORPLUGINIMPLEMENTATION_H
#define CIMINDICATORPLUGINIMPLEMENTATION_H

// Until aknstylusactivatedpopupcontent.h is fixed
class TAknWindowComponentLayout;

//  INCLUDES
#include <aknindicatorplugin.h>


/**
*  IM Indicator Plug-in implementation
*
*  @lib ImIndicatorPlugin
*  @since 3.2
*/
class CImIndicatorPluginImplementation : public CAknIndicatorPlugin

    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImIndicatorPluginImplementation* NewL();

        /**
        * Destructor.
        */
        ~CImIndicatorPluginImplementation();

    private: // From CAknIndicatorPlugin

        /**
         * @see CAknIndicatorPlugin
         */
        void HandleIndicatorTapL( const TInt aUid );

    public:// From CAknIndicatorPlugin

        /**
        * Returns the description used as link text, displayed with
        * the indicator icon in the indicator popup.
        *
        * @param       aUid       UID of the indicator.
        * @param[out]  aTextType  Type of the text,
        *                         @see TAknIndicatorPluginTextTypes.
        *
        * @return  Indicator description or link text.
        *          Ownership is transferred.
        */
        HBufC* TextL( const TInt aUid, TInt& aTextType );

    private:

        /**
        * C++ default constructor.
        */
        CImIndicatorPluginImplementation();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private: // new functions

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         *
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentMessageL();

        /**
        * Shows IM message(s)
        */
        void ShowMessageL();

    private: // data

        // Owns. Popup heading
        HBufC* iPopupHeading;

        // Owns. Popup content singular (template)
        HBufC* iPopupTextOne;

        // Owns. Popup content plural (template)
        HBufC* iPopupTextMultiple;

        // Owns. Actual content.
        HBufC* iContentText;

    };

#endif      // CIMINDICATORPLUGINIMPLEMENTATION_H

// End of File
