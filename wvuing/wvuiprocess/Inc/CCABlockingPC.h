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
* Description:  Process Component for Blocking and Unblocking
*
*/


#ifndef CCABLOCKINGPC_H
#define CCABLOCKINGPC_H

//  INCLUDES
#include "MCABackgroundObserver.h"
#include "MCABlockingPC.h"

//FORWARD DECLARATION
class MCABackgroundInterface;
class CCAEngine;
class MCABlocking;
class MCABlockingBackgroundObserverPC;
class MCAServerContactsArrayPC;
class CCAServerContactsArrayPC;

// CLASS DECLARATION
/**
 *  This class provides blocking functions to UI side
 *
 *  @lib chat.app
 *  @since 3.2
 */
class CCABlockingPC : public CBase,
            public MCABackgroundObserver,
            public MCABlockingPC

    {

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aBlockingInterface Reference to blocking interface
         * @param aBackgroundInterface Reference to aBackgroundInterface
         * @return Instance of the class
         */
        static CCABlockingPC* NewL( CCAEngine& aEngine );

        /**
         * Second phased symbian constructor.
         */
        void ConstructL();

        /**
         * Destructor
         */
        virtual ~CCABlockingPC();

    public:
        /**
         * Gets the backgroundstatus
         *@return the TCABackgroundStatus
         */
        TInt GetBackgroundStatus();

        /**
         * Gets the blockedlist from server
         *@return the
         */
        TInt GetBlockedListFromServerL();

        /**
         * Gets the retrieved blocked list
         *@return the blocked array
         */
        const CDesCArray* GetBlockedList();

        /**
         * Remove user from blocked or granted list.
         * @param aBlockList blocked to be removed entity's list.
         * @return error code.
         */
        TInt RemoveL( const CDesCArray* aUnBlockList,
                      const CDesCArray* aGrantList = NULL );

        /**
         * Insert users to be blocked or granted.
         * @param aBlockList blocked entity's list.
         * @param aGrantList granted entity's list.
         * @return error code.
         */

        TInt InsertL( const CDesCArray* aUnBlockList,
                      const CDesCArray* aGrantList );


        /**
         * Is this contact in our block list or not?
         * @param aContact specifies the contact.
         * @return ETrue if yes EFalse if not.
         */
        TBool IsContactBlocked( const TDesC& aContact,
                                TBool aUseGrant = EFalse ) const ;

        /**
          *  Set temporary grant on/off. If contact is blocked, unblock is permanent.
          *  @since 3.2
          *  @param aContact specifies the contact
          *  @param aOn On/off. ETrue = on
          *  @return errorCode if unsuccessfull
          */
        TInt SetTemporaryGrantL( const TDesC& aContact, TBool aOn ) ;

        /**
        * See MCABackgroundObserver
        */
        void HandleBackgroundEvent(
            MCABackgroundInterface::TCABackgroundTasks aEventSource,
            MCABackgroundInterface::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode
        );



        /**
         *Gets the list of blockedmembers
         */
        void GetBlockedMembersL( const TDesC& aIcon );

        /**
         * Gets the reference of ServerContactsArray
         */
        MCAServerContactsArrayPC& GetServerContactsArray();


        /**
         * Gets blocked members from Blocking interface and fetch nicknames also
         * for contact ID's from Storage.
         * @param aIcon Icon to be inserted in front of
         *              each list item (e.g "1\t")
         * @return contacts array with wvid's and nicknames
         */
        MCAServerContactsArrayPC* GetBlockedMembersLC( const TDesC& aIcon );


        /**
         * Gets the list to be unblocked
         * @Param aSelectedItem - selected index
         * @param aFindTxt - Find Pane Text
         */
        CDesCArrayFlat* GetUnBlockListL( TInt aSelectedItem,
                                         const TDesC& aFindTxt );


        /**
         * GetWvId:gets the WvId of a given index
         */
        TPtrC GetWvIdL( const TDesC& aIcon, TInt aDeletedIndex );

        /**
         * DeleteContactL: Deletes a contact
         * @Param aWvId: WVID of the contact to be deleted
         * @Param aFindTxt : Find Pane Text
         */
        TInt DeleteContactL( TInt aSelectedItem, const TDesC& aFindTxt );

        /**
         * RemoveIDsL: removes the list of userids to be unblocked
         * @Param aUserIds: list of userids
         */
        void RemoveIDsL( const CDesCArray* aUserIds );

    public://From MCABackgroundObserverPC

        /**
         * Registers the UI observer for backgorund tasks.
         * @param aBackgroundObserver: observer to be registered
         */
        void RegisterBackgroundObserver( MCABlockingBackgroundObserverPC*
                                         aBackgroundObserver );
        /**
         * UnRegisters the UI observer for backgorund tasks.
         *
         */
        void UnRegisterBackgroundObserver( );


    private:

        /**
        * Default Constructor
        * @param aEngine - reference to chat engine
        */
        CCABlockingPC( CCAEngine& aEngine );



    private://Data members

        CCAEngine& iEngine;

        //Pointer to blocking interface not owned so don't delete
        MCABlocking& iBlockingInterface;

        //Pointer to BackgroundInterface not owned so don't delete
        MCABackgroundInterface& iBackgroundInterface;

        //Owns. Pointer to CCAServerContactsarrayPC
        MCAServerContactsArrayPC* iBlockingArray;

        //Backgorund observer for ui to be notified of for
        //any background  activites received from engine
        MCABlockingBackgroundObserverPC* iBackgroundObserver;


    };
#endif //end of CCABlockingPC	   					  