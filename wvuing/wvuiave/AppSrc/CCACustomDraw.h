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
* Description:  Custom drawer class for invite viewer
*
*/



#ifndef CCACUSTOMDRAW_H
#define CCACUSTOMDRAW_H

//  INCLUDES
#include <frmtlay.h>

// CLASS DECLARATION
class CAknsBasicBackgroundControlContext;
class CCoeControl;

/**
*  Draws skins for invite viewer
*
*  @lib chat.app
*  @since 2.1
*/
class CCACustomDraw : public CBase, public MFormCustomDraw
    {
    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CCACustomDraw();

    public: // New functions

        /**
         * Set new viewing rectangle
         * @param aRect New viewing rectangle
         * @since 3.0
         */
        void SetRect( const TRect& aRect );

        /**
        * Sets the background graphic of text.
        *
        * @param aBgContext  Background context which is used for drawing
        * @param aParent  Parent from which to get context position
        * @since 3.0
        */
        void SetBackgroundContext( CAknsBasicBackgroundControlContext* aBgContext, CCoeControl* aParent );

    public: // Functions from base classes

        /**
         * @see MFormCustomDraw
         */
        void DrawBackground( const TParam& aParam, const TRgb& aBackground, TRect& aDrawn ) const;


    public:

        /**
         * C++ default constructor.
         */
        CCACustomDraw( TRect aViewRect );

    private: //Data

        TRect iViewRect;

        // Not owned. pointer to skinned background context
        CAknsBasicBackgroundControlContext* iBgContext;

        // Not owned. parent control for drawing the skin in correct place
        CCoeControl* iParent;
    };

#endif      // CCACUSTOMDRAW_H

// End of File
