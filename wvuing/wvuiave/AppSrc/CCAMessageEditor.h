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
* Description:  This class creates message editor
*
*/



#ifndef CCAMESSAGEEDITOR_H
#define CCAMESSAGEEDITOR_H

//  INCLUDES
#include <eikrted.h>
#include <eikedwin.h>
#include <aknutils.h>

// FORWARD DECLERATIONS

class CCAMessageExtensionsHandler;
class CAknsBasicBackgroundControlContext;
class MCATapEventObserver;
class CCAEditIndicator;

// CLASS DECLARATION

/**
 * Helper class to get editor content events.
 * @since S60 v3.2
 */
class MCAMessageEditorObserver
    {
    public: // Enumerations
        enum TEditorEvent
            {
            EEditorContentCleared = 0,
            EEditorFirstContentAdded
            };

    public: // New methods

        /**
         * Handle editor event.
         */
        virtual void HandleMessageEditorEventL( TEditorEvent aEvent ) = 0;

    protected:

        /**
         * Destructor for protection.
         */
        virtual ~MCAMessageEditorObserver() {};
    };

/**
*  Chat application's message editor
*
*  @lib chat.app
*  @since 1.2
*/
class CCAMessageEditor : public CEikRichTextEditor
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aMessageHandler Helper message handler for copy/paste feature.
        */
        CCAMessageEditor( CCAMessageExtensionsHandler& aMessageHandler );

        /**
        * Default Symbian OS constructor.
        * @param aParent Handle to parent control
        * @param aNumberOfLines Number of lines
        * @param aTextLimit Text limit
        * @param aEdwinFlags Edwin flags
        * @param aFontControlFlags Control flags
        * @param aFontNameFlags Font name flags
        */
        void ConstructL( const CCoeControl* aParent, TInt aNumberOfLines,
                         TInt aTextLimit, TInt aEdwinFlags,
                         TInt aFontControlFlags = EGulFontControlAll,
                         TInt aFontNameFlags = EGulNoSymbolFonts );

        /**
        * Destructor.
        */
        virtual ~CCAMessageEditor();

    public: // New functions

        /**
        * This method returns number of lines
        * @return Number of lines ( if editor is expanded or reduced )
        */
        TInt NumberOfLines( ) const;

        /**
         * This method clears the editor
         */
        void ResetL();

        /**
         * This method sets the font, that is used by editor
         * @param aFont desired for editor
         */
        void SetFontL( const CFont* aFont );

        /**
         * This method sets the font color, that is used by editor
         * @param aFontColor desired font color for the editor
         */
        void SetFontColor( const TRgb aFontColor );

        /**
         * This method sets the baseline of font
         * @param aBaseLine desired baseline
         */
        void SetBaseLine( const TInt aBaseLine );

        /**
         * This method deletes private icon in editor
         * @param aFormatHasChanged ETrue if format is changed
         * @param aSelection Selection if somethig is selected
         * @param aCode Event code
         * @param aIsPicture ETrue if picture is deleted
         */
        void DeletePictureL( TBool& aFormatHasChanged,
                             const TCursorSelection& aSelection, TBool aCode,
                             TBool aIsPicture );

        /**
         * This method sets the view rect. This is the "rect" where editor is located
         * @param aRect Parent control's view rect
         */
        void SetViewRect( const TRect& aRect );

        /**
         * Sets skinned background context for editor.
         * @param aForceUpdate ETrue causes full update of skin context,
         *                     ETrue used when display flipped from portrait to
         *                     landscape and vice versa.
         */
        void SetupSkinContextL( TBool aForceUpdate = EFalse );

        /**
         * Force editor update so that correct vertical alignment for smiley
         * icons will be used. Called by container which owns this editor.
         * @since 3.0
         */
        void RefreshEditorL();

        /**
         * Sets message editor observer.
         * @since S60 v3.2
         * @param aObserver Pointer to observer, ownership not transfered.
         */
        void SetMessageEditorObserver( MCAMessageEditorObserver* aObserver );

        /**
         * Set observer for handling tap events.
         * Supports only single observer.
         * @param aObserver observer to handle tap events
         * @param aId control id which is used by the observer to
         *        identify this object.
         */
        void SetTapObserver( MCATapEventObserver* aObserver, TUint aId );

    private:

        /**
         * Notifies message editor observer when needed.
         * @since S60 v3.2
         * @param aOldLen Editor content length before editing.
         * @param aCurLen Editor content current length
         */
        void NotifyObserverIfNeededL( TInt aOldLen, TInt aCurLen ) ;

    public: // from CCoeControl

        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * @see CCoeControl
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    public:
        /**
         * Shows edit indicator
         */
        void ShowIndicator();

        /**
         * Hides edit indicator
         */
        void HideIndicator();

        /**
         * Sets the active object which has called show/hide of
         * edit indicator
         */
        void SetActiveObject( CCAEditIndicator* aActiveObject );

        /**
         * ReSets the active object which has called show/hide of
         * edit indicator
         */
        void ReSetActiveObject();

    private: // from MEikCcpuEditor

        /**
         *  Handle needed extension modififications for cut feature.
         *  @since 2.6
         *  @see MEikCcpuEditor
         */
        void CcpuCutL();

        /**
         *  Handle needed extension modififications for copy feature.
         *  @since 2.6
         *  @see MEikCcpuEditor
         */
        void CcpuCopyL();

        /**
         *  Handle needed extension modififications for paste feature.
         *  @since 2.6
         *  @see MEikCcpuEditor
         */
        void CcpuPasteL();

    private: // from CEikRichTextEditor

        /**
         * @see CEikRichTextEditor
         */
        void EditObserver( TInt aStartEdit, TInt aEditLength );

    private:    // From MObjectProvider

        /**
         * @see MObjectProvider
         */
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    private:    // New helper methods

        /**
         * Constructs clipboard buffer if needed and
         * sets its content and selection correctly,
         * method called before copying or cutting.
         * @since S60 v3.1
         */
        void ConstructAndSetUpClipboardBufferL();

    private:

        TInt iBaseLine;
        TRgb iFontColor;
        TRect iViewRect;

        TAknLayoutRect iTextLine1;
        TAknLayoutRect iTextLine2;

        // Does not own. Extension handler for messages
        CCAMessageExtensionsHandler& iMessageHandler;

        // Owns
        CAknsBasicBackgroundControlContext* iBgContext;
        // Owns
        CAknsBasicBackgroundControlContext* iFgContext;

        // Not owned.
        MCAMessageEditorObserver* iEditorObserver;

        // Not owned. Pointer to observer
        MCATapEventObserver* iTapObserver;

        // ID which needs to be reported back to TapObserver
        TUint iTapControlId;

        // Owned. Editor to be used when copying or
        // cutting. Perform extension conversions in
        // in this buffer editor to avoid flickering
        // and crashes on UI.
        CEikEdwin* iClipboardBuffer;

        //doesnt own
        CCAEditIndicator* iEditIndicatorActiveObj;
    };

#endif      // CCAMESSAGEEDITOR_H

// End of File
