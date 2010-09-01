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
* Description:  Interface for settings related operations
*
*/


#ifndef CCACONTACTEDITPC_H
#define CCACONTACTEDITPC_H


// INCLUDES
#include <e32base.h>
#include "MCAContactEditPC.h"

class MCASettingsPC;
class MCAStoredContact;


/*
* CCAContactEditPC: This is main interface of contact editor on
* Process component side. Implements all the functions of MCAContactEditPC Interface.
*
* @lib wvuiprocessng.lib
* @since 3.2
*/
class CCAContactEditPC : public CBase, public MCAContactEditPC
    {
    public:


        /**
         *
         * Used to get the info of the current contact details
         * Before calling this function Initialise function should have been called.
         * Caller is responsible for the data allocated to pointers.
         * @Param aAliasEnabled - Flag to denote alias is enabled or not
         * @Param aNicknameEnabled - Flag to denote nickname was enabled or not
         * @Param iOwnData - ETrue denotes the contact to be edited is owndata
         * @Param iOrigId - Returns the allocated pointer for UserId
         * @Param iAlias -  Returns the allocated pointer for Alias
         * @Param iNickname - Returns the allocated pointer for Nickname
         * @Param iUserId - Returns the allocated pointer for UserId
         * @Param iIdentification - Returns the allocated pointer for Identification
         * @since 3.2
         */
        virtual void GetContactDetailsL(
            const TBool aAliasEnabled,
            const TBool aNicknameEnabled,
            TBool&  aOwnData,
            HBufC*& aOrigId,
            HBufC*& aAlias,
            HBufC*& aNickname,
            HBufC*& aUserId,
            HBufC*& aIdentification );

        /**
         *
         * Used to initialise the contact which is to be edited
         * @Param aCCAMainViewArrayPC - Ptr to the process component of Main ViewArray
         * @Param aIndex - Current Index of the contact that is to be edited in
         *						the Main View Array
         * @Param aOwnStatus - Reference to the caller variable - Etrue
         *
         * @since 3.2
         */
        virtual void InitialiseL(
            MCAMainViewArrayPC& aMainViewArrayPC,
            const TInt aIndex, TBool& aOwnStatus );

        /**
         *
         * Used to initialise the contact which is to be edited
         * @Param aWVID - wvid
         * @Param aListId - ListID
         * @Param aOwnStatus - Reference to the caller variable - Etrue
         *
         * @since 3.2
         */
        virtual void InitialiseL( const TDesC& aWVID,
                                  const TDesC& aListId, TBool& aOwnStatus );

        /**
         *
         * Used to Find whether a WVUID is availble and valid
         * @Param aCreateNew - EFalse if Edit contact and ETrue if add new contact
         * @Param aListId - List id
         * @Param aWVID - WVID of the contact to be edited
         * @Param aContactInList - Returns ETrue/EFalse if the aWVID is available
         * @Param aUserId - returns the user id that s avilable in the store.
         * @Return ETrue/EFalse
         * @since 3.2
         */
        virtual TBool IsWVIDAvailableAndValidL( TBool aCreateNew, const TDesC& aListId,
                                                const TDesC& aWVID,
                                                TBool& aContactInList, TDesC*& aUserID );


        /**
         *
         * Used to Find whether any other contact has the same nickname
         * @Param aNickName - Nickname
         * @Return ETrue/EFalse
         * @since 3.2
         */
        virtual TBool FindAnyContactByNickL( const TDesC& aNickName )  ;


        /**
         * Add new contact to contact list (server)
         * @param aListId List id
         * @param aWVID WVID of the contact to be added
         * @param aNickName Nick name
         * @param aAlias AliasName
         * @return error code
         * @since 3.2
         */
        virtual TInt AddContactL( const TDesC& aListId,
                                  const TDesC& aWVID, const TDesC& aNickName,
                                  const TDesC& aAlias );


        /**
         * Update contact to contact list (server)
         * @param aListId List id
         * @param aWVID WVID of the contact to be updated
         * @param aNickName Nick name
         * @return error code
         * @since 3.2
         */
        virtual TInt UpdateContactL( const TDesC& aListId,
                                     const TDesC& aWVID,
                                     const TDesC& aNickName );

        /**
         * Remove contact from contact list (server)
         * @param aListId List id
         * @param aWVID WVID of the contact to be added
         * @return error code
         * @since 3.2
         */
        virtual TInt RemoveContactL( const TDesC& aListId,
                                     const TDesC& aWVID );


        /**
        * Sets the iContact to the contact found using aListId, aWVId
        * @param aListId List id
        * @param aWVID WVID of the contact
        * @since 3.2
        */
        virtual void SetContactL( const TDesC& aListId,
                                  const TDesC& aWVID );

        /**
         * Sets the iContact to the contact found using aWVId
         * @param aWVID WVID of the contact
         * @since 3.2
         */
        virtual void SetAnyContactL( const TDesC& aWVID ) ;

        /**
         * GetCurrentContactNickname
         * @return Nickname of this contact
         * @since 3.2
         */
        virtual const TDesC& GetCurrentContactNickname() const ;

        /**
         * GetCurrentUserId
         * @return UserId of this contact
         * @since 3.2
         */
        virtual const TDesC& GetCurrentContactUserId() const ;


        /**
         * Online status of user.
         * @param aContactId Id which status is wanted.
         * @return Onlinestatus of user.
         * @since 3.2
         */
        virtual TEnumsPC::TOnlineStatus GetOnlineStatus() const;

        /**
         * Client type
         * @return Client type of this contact
         * @since 3.2
         */
        virtual TEnumsPC::TClientType GetClientType() const;

        /**
         * Status Text
         * @return StatusText of the current contact
         * @since 3.2
         */
        virtual const TDesC& GetStatusText() const;

        /**
         * Identification Name
         * @return Identification of the current contact
         */
        virtual const TDesC& GetCurrentContactIdentification() const;

        /**
         * Alias Name
         * @return Alias of the current contact
         * @since 3.2
         */
        virtual const TDesC& GetCurrentContactAlias() const;


        /**
         * allocates and returns a pointer to the aliasname
         * @param aWVID WVID of the contact
         * @return pointer - This must be freed by the caller.
         * @since 3.2
         */
        virtual HBufC* GetAliasFromPresenceL( const TDesC& aWVID );

        /**
         * Deletes currently viewed contact.
         * @return General error code or KErrNone.
         * @since 3.2
         */
        virtual TInt DeleteCurrentContactL( const TDesC& aListId,
                                            const TDesC& aOrigId );



        /**
         * Get the error of the last performed contact operation
         * @return an Array by reference.
         * @since 3.2
         */
        virtual const RPointerArray<MCAPresenceError>&
        LastContactEditOperationResultL();



        /**
         * Is Contact
         * @return ETrue - if contact available else EFalse.
         * @since 3.2
         */
        virtual TBool IsContact() const;

        /**
         * Allocates and returns the ListId for the given UserId
         * @param aId - Current User Id in case of operation other than move
         *				- Current listid in case the operation is move
         * @param aNoListsResource
         * @param aItemCount - returns itemcount of the found listId's
         * @param aListIdArray - returns a pointer to the array of listId's
         * @param aMoveOperation - Bool to represent whether it is a Move operation/not
         * @return Pointer to the list id if exists else NULL.
         * The return pointer has to be freed by the caller.
         * @since 3.2
         */
        virtual HBufC* GetListIdL( const TDesC& aId, TInt aNoListsResource,
                                   TInt &aItemCount, CDesCArrayFlat*& aListIdArray,
                                   RArray< TInt > 	&aIndexArray,
                                   const TBool aMoveOperation = EFalse );

        /**
         * Allocates and returns the ListId for the given UserId
         * This function should be preceded by GetListIdL
         * Do not call this fnction without calling GetListIdL and item count > 1
         * @param aSelectedIndex - Selected index
         * @return Pointer to the list id
         * The return pointer has to be freed by the caller.
         * @since 3.2
         */
        virtual HBufC* GetListIdAtIndexL( TInt aSelectedIndex ) const ;


        /**
        * Changes own alias to aAlias
        * @param aAlias New own alias
        * @return error code
        * @since 3.2
        */

        virtual TInt UpdateOwnAliasL( const TDesC & aAlias ) ;


        /**
        * Verify that contact is still valid.
        * @param aListId - List ID
        * @return ETrue - valid user
        *         EFalse - invalid user
        * @since 3.2
        */
        virtual TBool IsContactValidL( const TDesC& aListId );

        /**
        * ReleaseResources
        * @Reset the iContact and iOwnData
        */
        virtual void ReleaseResources();

        /**
        * IsOwnData
        * return ETrue if the current contact is OwnData
        */
        virtual TBool IsOwnData() const;

        /**
         * Is list collapsed or not.
         * @return ETrue, if collapsed, EFalse if expanded
         */
        virtual TBool IsCollapsedL( const TDesC& aListId );

        /**
         * DeleteContactL: Deletes a contact
         * @Param aBlockedContact: WVID of the contact to be deleted
         * retruns errorcode.
         */
        virtual TInt DeleteContactL( const TDesC& aBlockedContact );

        /**
        * NewL Functions.
        * @since 3.2
        */
        static CCAContactEditPC* NewL( MCASettingsPC& aCCASettingsPC );
        static CCAContactEditPC* NewLC( MCASettingsPC& aCCASettingsPC );

        /**
        * Destructor
        * @since 3.2
        */
        virtual ~CCAContactEditPC();

    private:
        /**
        * Constructor
        * @since 3.2
        */
        CCAContactEditPC( MCASettingsPC& aCCASettingsPC );

    private:

        // Does not own. Contact pointer reference.
        MCAStoredContact* 	iContact;

        //ETrue - OwnData
        TBool iOwnData;

        //does not own. Reference to settings interface
        MCASettingsPC& iCCASettingsPC;


    };


#endif // CCACONTACTEDITPC_H

// End of File
