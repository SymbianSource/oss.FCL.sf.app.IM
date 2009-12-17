/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Takes care of persistent storaging for groups
 *
*/


#ifndef CCAGROUPSTORAGE_H
#define CCAGROUPSTORAGE_H


//  INCLUDES

#include <e32base.h>
#include <e32std.h>

#include <s32file.h>
#include <d32dbms.h>

// FORWARD DECLARATIONS

class MCAExtendedStoredGroup;

// CLASS DECLARATION
/**
 * Implements database storage routines for groups
 *
 * @lib CAStorage.dll
 * @since 2.1
 */
class CCAGroupStorage : public CBase
    {
    public:

        /**
         * Destructor.
         */
        ~CCAGroupStorage();

        /**
         * Two-phased constructor.
         */
        static CCAGroupStorage* NewL();

        /**
         * Sets database that group storage uses.
         * @since	2.1
         * @param	aDb	Database that is used.
         */
        void SetDbL( RDbStoreDatabase aDb );

        /**
         * Reads first group from storage
         * @since	2.1
         * @param	aGroup		Group is read to this object.
         * @return	KErrNone,	if successful.
         */
        TInt ReadFirstL( MCAExtendedStoredGroup* aGroup );

        /**
         * Reads next group from storage
         * @since	2.1
         * @param	aGroup		Group is read to this object.
         * @return	KErrNone,	if successful.
         */
        TInt ReadNextL( MCAExtendedStoredGroup* aGroup );

        /**
         * Reads group from storage
         * @since	2.1
         * @param	aGroup		Group is read to this object.
         */
        void ReadGroupL( MCAExtendedStoredGroup* aGroup );

        /*
         * Writes group to storage.
         * @since	2.1
         * @param	aGroup	Group to write to storage.
         */
        void WriteGroupL( MCAExtendedStoredGroup* aGroup );

        /*
         * Deletes group from storage
         * @since	2.1
         * @param	aGroup	Group to delete.
         */
        void DeleteGroupL( MCAExtendedStoredGroup* aGroup );

    protected:

        /**
         * C++ default constructor.
         */
        CCAGroupStorage();

    private:

        /**
         * Opens database table
         * @since	2.1
         */
        void OpenTableL();

        /**
         * Closes database table
         * @since	2.1
         */
        void CloseTable();

        /**
         * Write the given group to the database file
         * @since	2.1
         * @param aGroup The group
         */
        void WriteToStoreDbL( MCAExtendedStoredGroup* aGroup );

        /*
         * Read current group from database
         * @since	2.1
         * @param	aGroup	Contact to fill with data
         */
        void ReadCurrentL( MCAExtendedStoredGroup* aGroup );

        /**
         * Seek a specific row with given group id in the database file.
         * @since	2.1
         * @param aColNo The column number (KGroupId)
         * @param aId The group id to find
         * @return ETrue if seek successful, otherwise EFalse
         */
        TBool SeekRowL( TDbColNo aColNo, const TDesC& aId );

    private:
        // database
        RDbStoreDatabase	iDb;

        // current column set
        CDbColSet*			iColset;		// owned

        TBool				iTableOpened;   // is table opened
        RDbTable			iTable;         // current table
    };

#endif // CCAGROUPSTORAGE_H

//  End of File
