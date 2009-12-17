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
* Description: Process Component Interface for Blocking and Unblocking
*
*/


#ifndef MCABLOCKINGPC_H
#define MCABLOCKINGPC_H


//includes

//Calss declarations
class MCAServerContactsArrayPC;
class MCABlockingBackgroundObserverPC;

class MCABlockingPC
    {

    public:
        /**
         * Gets the backgroundstatus
         *@returns the TCABackgroundStatus
         */
        virtual TInt GetBackgroundStatus( ) = 0;

        /**
         * Gets the blockedlist from server
         *@returns the
         */
        virtual TInt GetBlockedListFromServerL() = 0;

        /**
         * Gets the retrieved blocked list
         * @return the blocked array
         */
        virtual const CDesCArray* GetBlockedList() = 0;

        /**
         * Remove user from blocked or granted list.
         * @param aBlockList blocked to be removed entity's list.
         * @return error code.
         */
        virtual TInt RemoveL( const CDesCArray* aUnBlockList,
                              const CDesCArray* aGrantList /*=NULL*/ ) = 0;

        /**
         * Insert users to be blocked or granted.
         * @param aBlockList blocked entity's list.
         * @param aGrantList granted entity's list.
         * @return error code.
         */
        virtual TInt InsertL( const CDesCArray* aUnBlockList,
                              const CDesCArray* aGrantList ) = 0;


        /**
         *Gets the list of blockedmembers
         */
        virtual void GetBlockedMembersL(
            const TDesC& aIcon ) = 0;


        /**
         * Gets the reference of ServerContactsArray
         */
        virtual MCAServerContactsArrayPC& GetServerContactsArray() = 0;

        /**
         * Gets blocked members from Blocking interface and
         * fetch nicknames also
         * for contact ID's from Storage.
         * @param aIcon Icon to be inserted in front of
         *              each list item (e.g "1\t")
         * @return contacts array with wvid's and nicknames
         */
        virtual MCAServerContactsArrayPC* GetBlockedMembersLC(
            const TDesC& aIcon ) = 0;

        /**
         * Gets the list to be unblocked
         * @Param aSelectedItem - selected index
         * @param aFindTxt - Find Pane Text
         */
        virtual CDesCArrayFlat* GetUnBlockListL( TInt aSelectedItem,
                                                 const TDesC& aFindTxt ) = 0;

        /**
         * GetWvId: gets the Wvid od a given index.
         */
        virtual TPtrC GetWvIdL( const TDesC& aIcon,
                                TInt aDeletedIndex ) = 0;


        /**
         * RegisterBackgroundObserver to register for background handling
         */
        virtual void RegisterBackgroundObserver(
            MCABlockingBackgroundObserverPC*
            aBackgroundObserver ) = 0;
        /**
         * UnRegisterBackgroundObserver to unregister for background handling
         */
        virtual void UnRegisterBackgroundObserver(  ) = 0;

        /**
         * DeleteContactL: Deletes a contact
         * @Param aWvId: WVID of the contact to be deleted
         * @Param aFindTxt : Find Pane Text
         */
        virtual TInt DeleteContactL( TInt aSelectedItem,
                                     const TDesC& aFindTxt ) = 0;

        /**
         * RemoveIDsL: removes the list of userids to be unblocked
         * @Param aUserIds: list of userids
         */
        virtual void RemoveIDsL( const CDesCArray* aUserIds ) = 0;


        /**
         * Is this contact in our block list or not?
         * @param aContact specifies the contact.
         * @return ETrue if yes EFalse if not.
         */
        virtual TBool IsContactBlocked( const TDesC& aContact,
                                        TBool aUseGrant = EFalse ) const = 0;


        /**
        *  Set temporary grant on/off. If contact is blocked, unblock is permanent.
        *  @since 3.2
        *  @param aContact specifies the contact
        *  @param aOn On/off. ETrue = on
        *  @return errorCode if unsuccessfull
        */
        virtual TInt SetTemporaryGrantL(
            const TDesC& aContact, TBool aOn ) = 0;

    public:
        /**
        *virtual Destructor
        */
        virtual ~MCABlockingPC() {};

    };
#endif //end of file