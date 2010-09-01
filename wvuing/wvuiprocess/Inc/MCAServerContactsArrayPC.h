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



#ifndef MCASERVERCONTACTSARRAYPC_H
#define MCASERVERCONTACTSARRAYPC_H

//  INCLUDES
#include <bamdesca.h>
#include <badesca.h>
#include "ChatDefinitions.h" //KTab

// FORWARD DECLARATIONS

class MCAStoredContact;
class MDesCArray;

// Helper struct for holding the identification, nickname and wvid of contact
// contains also information about nicname existense in phone for some cases.
struct TCAServerContact
    {
    TPtrC iIdentification;
    TPtrC iNickName;
    TPtrC iWVID;
    TBool iExistInPhone;
    };



// CLASS DECLARATION

/**
*  This class is decorated array for list box
*
*  @lib chat.app
*  @since 2.1
*/
class MCAServerContactsArrayPC: public MDesCArray
    {


    public: // New functions


        /**
         * Sorts the array
         */
        virtual void Sort() = 0;

        /**
         * Adds an item to iNickNames and iWVIds arrays.
         * @param aNickName nickname of the contact. can be KNullDesC
         * @param aWVId WV id of contact. mandatory
         */
        virtual void AddL( const TDesC& aNickName, const TDesC& aWVId ) = 0;

        /**
         * Remove an item from given index
         */
        virtual void Remove( TInt aIndex ) = 0;

        /**
         * Returns an index of given contact or KErrNotFound
         */
        virtual TInt Find( const TDesC& aWVId ) const = 0;

        /*
         * Returns the array of wv ids
         */
        virtual const MDesCArray& WVIDArray() const = 0;

        /**
         * Returns single contact from servercontacts array.
         * @param aIndex
         * @param aFindBoxTxt - Text in the findPane (Added for Fixing

         */
        virtual const TCAServerContact* WVIdL( TInt aIndex,
                                               const TDesC& aFindBoxTxt = KNullDesC ) const = 0;

        /*
         * Returns the array of nicknames
         * corresponds to WVIDArray
         */
        virtual const MDesCArray& NickNameArray() const = 0;

        virtual void ReplaceL( CDesCArray* aNickNames,
                               CDesCArray* aWVIds,
                               const TDesC& aIconTabulator,
                               CDesCArray* aIdentifications = NULL ) = 0;

        /**
        * From MDesCArray Returns the number of descriptor
        * elements in a descriptor array
        * @return The number of descriptor elements in a descriptor array
        */
        virtual TInt MdcaCount() const = 0;

        /**
         * From MDesCArray Indexes into a descriptor array
         * @param aIndex The position of the descriptor element within a
         *		  descriptor array. The position is relative to zero; i.e. zero
         *        implies the first descriptor element in a descriptor array.
         * @return A 16 bit non-modifiable pointer descriptor representing the
         *		   descriptor element located at position aIndex within a
         *		   descriptor array.
         */
        virtual TPtrC MdcaPoint( TInt aIndex ) const = 0;

    public:
        /**
        * Destructor.
        */
        virtual ~MCAServerContactsArrayPC() { };



    };

#endif      // MCAServerContactsArrayPC

// End of File
