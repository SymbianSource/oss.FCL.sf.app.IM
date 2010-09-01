/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for text viewer
*
*/



#ifndef MCATEXTVIEW_H
#define MCATEXTVIEW_H

#include <frmtlay.h> // TCursorSelection

// CLASS DECLARATION
class TPointerEvent;

/**
*  Interface for text viewer
*
*  @lib chat.app
*  @since Series 60 3.0
*/
class MCATextView
    {
    public: // Enumerations

        enum TPenBehaviour
            {
            EPenForwardToChild = 0,
            EPenForwardToParent,
            EPenProcessEvent,
            EPenIgnoreEvent
            };

    public: // New functions

        /**
         * Handles changed format.
         * @param aChanged Changed section of rich text
         * @param aPreserveFocus ETrue : Keeps focus of text control in place
         *                       EFalse : Focus moves with changes
         */
        virtual void HandleFormatChangedL( TCursorSelection& aChanged,
                                           TBool aPreserveFocus = EFalse ) = 0;

        /**
         * Handles insertion and deletion
         * @param aChanged Changed section of rich text. If this function is
         *                 being used to handle deletion only, this argument
         *                 should be of length zero and its start position
         *                 should be the beginning of the deletion.
         * @param aDeletedChars The number of deleted characters. Specify zero
         *                 if this function is being used to handle insertion
         *                 only.
         */
        virtual void HandleInsertDeleteL( TCursorSelection& aChanged,
                                          TInt aDeletedChars ) = 0;

        /**
         * Handles addition of text
         * @param aFirst Should be set to ETrue if
         *               this is the first addition of text
         * @param aSelection Selection
         * @param aTop If selection doesn't fit to screen, should the top of the
         *        selection be visible. I.e. If ETrue, top of the message is shown.
         *        if EFalse, bottom of the message is shown.
         */
        virtual void HandleAdditionL( TBool aFirst, TCursorSelection& aSelection,
                                      TBool aTop ) = 0;

        /**
         * Scrolls selection to screen and updates scroll bars
         * @param aSelection Selection
         * @param aTop If selection doesn't fit to screen, should the top of the
         *        selection be visible. I.e. If ETrue, top of the message is shown.
         *        if EFalse, bottom of the message is shown.
         */
        virtual void ScrollVisibleL( TCursorSelection& aSelection, TBool aTop ) = 0;

        /**
         * @return Current scroll selection (i.e. the visible part)
         */
        virtual TCursorSelection ScrollSelection() = 0;

        /**
         * Scrolls view up or down
         * @param aAmount Amount of lines to be scrolled
         * Use negative values for scrolling up
         */
        virtual void ScrollLinesL( TInt aAmount ) = 0;

        /**
         * @return ETrue if aPos is visible on screen, EFalse otherwise.
         */
        virtual TBool IsVisible( TInt aPos ) = 0;

        /**
         * @return ETrue if the paragraph in aPos fits to screen
         */
        virtual TBool ParagraphFits( TInt aPos ) = 0;

        /**
         * Redraws the view
         */
        virtual void Redraw() = 0;

        /**
         * Sets the behaviour of this control to pointer events.
         * see TPenBehaviour for more detailed info.
         * @param aPen How this control should react to pointer events
         */
        virtual void SetPenBehaviour( TPenBehaviour aPen ) = 0;

        /**
         * Return current pen behaviour state. Determines how this control
         * reacts to pointer events.
         * @return Current pen behaviour
         */
        virtual TPenBehaviour PenBehaviour() = 0;

        /**
         * Reformats the view after a global change has been made to the layout,
         * but without causing a redraw.
         * @since S60 v3.2
         * @param aYPosQualifier Specifies whether the visible area is to be
         *                       filled and whether the top line should be made
         *                       fully visible if possible.
         */
        virtual void HandleGlobalChangeNoRedrawL(
            TViewYPosQualifier aYPosQualifier = TViewYPosQualifier() ) = 0;

    protected:

        /**
         * This gets called when the pointer event is inside the text message.
         * @param aPointerEvent Pointer event
         */
        virtual void ProcessStylusEventL(
            const TPointerEvent& aPointerEvent ) = 0;

        /**
         * Destructor.
         */
        virtual ~MCATextView() {};

    };

#endif      // MCATEXTVIEW_H

// End of File
