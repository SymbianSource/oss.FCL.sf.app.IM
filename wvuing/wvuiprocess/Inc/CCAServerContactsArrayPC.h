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
* Description:  Declares server contacts array
*
*/



#ifndef CCASERVERCONTACTSARRAYPC_H
#define CCASERVERCONTACTSARRAYPC_H

//  INCLUDES
#include <bamdesca.h>
#include <badesca.h>
#include "ChatDefinitions.h" //KTab
#include "MCAServerContactsArrayPC.h"

// FORWARD DECLARATIONS

class MCAStoredContact;


// CLASS DECLARATION

/**
*  This class is decorated array for list box
*
*  @lib wvuiprocessng.dll
*  @since 3.1
*/

class CCAServerContactsArrayPC : public CBase,
            public MCAServerContactsArrayPC
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor. Leaves with KErrArgument if any of the
         * arguments are NULL or they have different number of items.
         * Ownership of arguments are transferred only if construction of
         * the object is fully successfull.
         * @param aNickNames list of nicknames. Takes the ownership.
         * @param aWVIds list of WV-ids. Takes the ownership.
         * @param aIconTabulator Tabulator which is put before identification
         *                       when returning MdcaPoint
         * @param aShowContactExistInPhoneIcon Tells if icon should be shown
         *                                     on D-column, when contact
         *                                     already exists in phone.
         * @return New instance of server contacts array
         */
        static CCAServerContactsArrayPC* NewL( CDesCArray* aNickNames,
                                               CDesCArray* aWVIds,
                                               const TDesC& aIconTabulator = KTab,
                                               CDesCArray* aIdentifications = NULL,
                                               TBool aShowContactExistInPhoneIcon = EFalse
                                             );

        /**
         * Two-phased constructor.
         * Ownership of arguments are not transferred.
         * @param aContacts List of the contacts
         * @param aIconTabulator Tabulator which is put before identification
         *                       when returning MdcaPoint
         * @param aShowContactExistInPhoneIcon Tells if icon should be shown
         *                                     on D-column, when contact
         *                                     already exists in phone.
         * @return New instance of server contacts array
         */
        static CCAServerContactsArrayPC* NewL(
            RPointerArray<MCAStoredContact>& aContacts,
            const TDesC& aIconTabulator = KTab,
            TBool aShowContactExistInPhoneIcon = EFalse );

        /**
         * Destructor.
         */
        virtual ~CCAServerContactsArrayPC();

    public: // Functions from MDesCArray

        /**
         * From MDesCArray Returns the number of descriptor
         * elements in a descriptor array
         * @return The number of descriptor elements in a descriptor array
         */
        virtual TInt MdcaCount() const;

    public: // Functions from MDesCArray

        /**
         * From MDesCArray Indexes into a descriptor array
         * @param aIndex The position of the descriptor element within a
         *		  descriptor array. The position is relative to zero; i.e. zero
         *        implies the first descriptor element in a descriptor array.
         * @return A 16 bit non-modifiable pointer descriptor representing the
         *		   descriptor element located at position aIndex within a
         *		   descriptor array.
         */
        virtual TPtrC MdcaPoint( TInt aIndex ) const;

    public: // New functions



        /*
         * Inlined function separated in own file.
         *
         * inline const TCAServerContact& operator [] (TInt aIndex) const
         * { return iServerContacts[ aIndex ]; }
         */
        //const TCAServerContact& operator [] (TInt aIndex) const
        //{ return iServerContacts[ aIndex ]; }
#include "CCAServerContactsArrayPC.inl"

        /**
         * Sorts the array
         */
        virtual void Sort();

        /**
         * Adds an item to iNickNames and iWVIds arrays.
         * @param aNickName nickname of the contact. can be KNullDesC
         * @param aWVId WV id of contact. mandatory
         */
        virtual void AddL( const TDesC& aNickName, const TDesC& aWVId );

        /**
         * Remove an item from given index
         */
        virtual void Remove( TInt aIndex );

        /**
         * Returns an index of given contact or KErrNotFound
         */
        TInt Find( const TDesC& aWVId ) const;

        /**
         * Returns the array of wv ids
         */
        const MDesCArray& WVIDArray() const;

        /**
         * Returns single contact from servercontacts array.
         * @param aIndex
         * @param aFindBoxTxt - Text in the findPane (Added for Fixing

         */
        const TCAServerContact* WVIdL( TInt aIndex,
                                       const TDesC& aFindBoxTxt = KNullDesC ) const;

        /**
         * Returns the array of nicknames
         * corresponds to WVIDArray
         */
        const MDesCArray& NickNameArray() const;

        /**
         * Replaces the existitn nicknemes,wvids with the new ones.
         * @param aNickNames: New set of nickenames to be set with.
         * @param aWVIds: New set of wvid's to be set with.
         */
        void ReplaceL( CDesCArray* aNickNames,
                       CDesCArray* aWVIds,
                       const TDesC& aIconTabulator,
                       CDesCArray* aIdentifications = NULL );

    private:

        /**
         * C++ default constructor.
         * @param aShowContactExistInPhoneIcon Tells if icon should be shown on
         *                                     D-column, when contact
         *                                     already exists in phone.
         */
        CCAServerContactsArrayPC( TBool aShowContactExistInPhoneIcon );

        /**
         * By default Symbian OS constructor is private.
         * @param aNickNames list of nicknames. Takes the ownership.
         * @param aWVIds list of WV-ids. Takes the ownership.
         * @see NewL
         */
        void ConstructL( CDesCArray* aNickNames,
                         CDesCArray* aWVIds,
                         const TDesC& aIconTabulator,
                         CDesCArray* aIdentifications = NULL );

        /**
         * By default Symbian OS constructor is private.
         * @see NewL
         */
        void ConstructL( RPointerArray<MCAStoredContact>& aContacts,
                         const TDesC& aIconTabulator );

        /**
         * common parts of the ConstructL methods
         * @see ConstructL
         */
        void CommonConstructL( const TDesC& aIconTabulator );

    private:    // Data

        //Owns. Array of nicknames.
        CDesCArray* iNickNames;

        //Owns. Array of identifications.
        CDesCArray* iIdentifications;

        //Owns. Array of wvid's
        CDesCArray* iWVIds;

        //Array of server contacts.
        RArray<TCAServerContact> iServerContacts;

        //owns
        HBufC* iLine;

        HBufC* iIconTabulator;

        // helper variable to know when contact existence in phone
        // should be checked, and related icon shown in D-column of listbox
        TBool iShowContactExistInPhoneIcon;

    };

#endif      // CCASERVERCONTACTSARRAY_H

// End of File
