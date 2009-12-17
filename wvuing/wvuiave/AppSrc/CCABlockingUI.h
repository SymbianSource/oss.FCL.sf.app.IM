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
* Description:  This class is UI side blocking handler
 *
*/



#ifndef CCABLOCKINGUI_H
#define CCABLOCKINGUI_H

//  INCLUDES
#include "MCABlocking.h"
#include "MCABlockingBackgroundObserverPC.h"
#include "MCABlockingPC.h"
#include "MCAMainViewArrayPC.h"

#include <EIKCMOBS.H> //MEikCommandObserver

// CLASS DECLARATION
class MCABackgroundInterface;
class CCAAppUi;
class MCAServerContactsArrayPC;
class CCABlockingDialog;
/**
 *  This class provides blocking functions to UI side
 *
 *  @lib chat.app
 *  @since 1.2
 */


class CCABlockingUI : public CBase,
            MCABlockingBackgroundObserverPC, MEikCommandObserver
    {
    private:
        enum TCAOperationWaiting
            {
            //add or removed
            EUnknown = 0,
            EShowBlockList,
            EShowUnblockList
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aBlockingPC Pointer to blockinginterfacee
         * @return Instance of the class
         */
        static CCABlockingUI* NewL( MCABlockingPC* aBlockingPC );

        /**
         * Two-phased constructor. Leaving pointer to stack
         * @param aBlockingPC Pointer to blockinginterface
         * @return Instance of the class
         */
        static CCABlockingUI* NewLC( MCABlockingPC* aBlockingPC );

        /**
         * Destructor.
         */
        virtual ~CCABlockingUI();

    public: // New functions

        /**
         * Displays blocked list
         * @param aDontShowTabGroup. For handling navipane.
         * @since 1.2
         */
        void DisplayBlockedListL( TBool aDontShowTabGroup = EFalse );

        /**
         * Displays selection list of blocked members.
         */
        void DisplayUnblockListQueryL();

        /**
         * Displays text query dialog where user can set blocked WV ID
         */
        void DisplayBlockTextQueryL();

        /**
         * Blocks given user
         * @param aUserId user id of blocked member
         */
        void BlockUserL( const TDesC& aUserId );

        /**
         * Blocks given users
         * @param aUserIds array of user id of blocked members
         */
        void BlockMultipleUsersL( const CDesCArray* aUserIds );

        /**
         * Restore teh title
         */
        static void RestoreTitle( TAny* aInstance );

        void DoRestoreTitleL();

        /**
         * Displays selection list of blocked members.
         */
        void DisplayBlockListQueryL();

        /**
         * Unblocks one user, aUserId.
         * @since S60 v3.1
         * @param aUserId WV id of contact to be unblocked.
         */
        void UnblockUserL( const TDesC& aUserId );

        // ---------------------------------------------------------
        // void CCASingleListContainer::GetHelpContext()
        // Gets help context
        // ---------------------------------------------------------
        //
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    public: //From MCABlockingBackgroundObserverPC

        /**
         * From MCABlockingBackgroundObserverPC
         * @see MCABlockingBackgroundObserverPC
         */
        void HandleBackgroundEventPC(
            TEnumsPC::TCABackgroundTasks aEventSource,
            TEnumsPC::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode );

    public:	//from MEikCommandObserver
        /**
         * From MEikCommandObserver
         * @see MEikCommandObserver
         */
        void ProcessCommandL( TInt aCommandId );

        /**
         * From MEikCommandObserver
         * @see MEikCommandObserver
         */
        CCoeControl* CreateCustomCommandControlL( TInt aControlType );

    private:

        /**
         * C++ default constructor.
         * @param aBlockingInterface Reference to blocking interface
         * @param aBackgroundInterface Reference to aBackgroundInterface
         */
        CCABlockingUI( MCABlockingPC* aBlockingPC ); //MCABlocking& aBlockingInterface,
        //	MCABackgroundInterface&  aBackgroundInterface );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * UnBlocks a SINGLE CONTACT
         */
        void UnBlockL();

        /**
         * Takes the backup of title.
         */
        void BackupTitleL();

        /**
         * Displays text query dialog where user can set blocked WV ID
         * for ProcessCommandL
         */
        void DisplayBlockTextQuery2L();

        /**
         * Removed user ids from all lists.
         * @param aUserIds array of ids to remove.
         */
        void RemoveIDsL( const CDesCArray* aUserIds ) const;

        /**
         * Sets correct list box index after blocking of new
         * contact(s).
         * @since S60 v3.1
         * @param aMultipleBlocked If ETrue focus is set to first
         *                         list box item.
         * @param aBlockedId If aMultipleBlocked is EFalse and
         *                   aBlockedId is given focus is set to that.
         */
        void SetIndexAfterBlocking( TBool aMultipleBlocked = EFalse,
                                    const TDesC& aBlockedId = KNullDesC );

    private:    // Data

        //Operation waiting to complete
        TCAOperationWaiting iFetchOperation;

        //Doesn't own
        MCABlockingPC* iBlockingPC;

        //Doesn't own
        CCAAppUi* iAppUi;


        TBool iWaiting;

        //Owns. Pointer to BlockingDialog
        CCABlockingDialog* iBlockingDialog;


        TInt iSelectedItem;

        //owns
        HBufC* iOldTitle;

        // EFalse, if numeric input for WVID fields is wanted
        TBool iTextualInputMode;

        //Does not own. pointer to MainViewArrayPC
        MCAMainViewArrayPC* iMainViewArrayPC;

        //Does not own
        MCAServerContactsArrayPC* iBlockingArray;

        // to avoid application crash when  bolck dialog closed
        // by AknDialogShutter::ShutDialogsL( *iEikonEnv )
        TBool iDialogShutter;

    };

#endif      // CCABLOCKINGUI_H

// End of File
