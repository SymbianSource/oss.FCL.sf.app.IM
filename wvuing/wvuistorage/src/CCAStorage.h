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
* Description:  Class for persistent storaging
*
*/



#ifndef CCASTORAGE_H
#define CCASTORAGE_H

//  INCLUDES

#include <e32base.h>
#include <e32std.h>

#include <s32file.h>
#include <d32dbms.h>

#include "CCAGroupStorage.h"

// FORWARD DECLARATIONS

class MCAExtendedStoredContact;
class MCAExtendedStoredGroup;

// CLASS DECLARATION
/**
 * This class handles the persistent storaging, i.e. interfacing with the
 * database. It offers an internal interface to the database for the
 * CCAStorageManager.
 *
 * @lib CAStorage
 * @since 2.1
 */
class CCAStorage : public CBase
    {
    public:

        /**
         * Destructor
         */
        ~CCAStorage();

        /**
         * Two-phased constructor.
         */
        static CCAStorage* NewL();

        /*
         * Loads group
         * @since	2.1
         * @param	aGroup	Group is read to this object
         */
        void	LoadL( MCAExtendedStoredGroup* aGroup );

        /*
         * Saves group
         * @since	2.1
         * @param	aGroup	Group that is saved
         */
        void	SaveL( MCAExtendedStoredGroup* aGroup );

        /*
         * Deletes group
         * @since	2.1
         * @param	aGroup	Group that is deleted
         */
        void	DeleteL( MCAExtendedStoredGroup* aGroup );

        /*
         * Loads first group from database
         * @since	2.1
         * @param	aGroup	Group is read to this object
         * @return	KErrNone, if ok. KErrNotFound, if group is not found.
         */
        TInt	LoadFirstL( MCAExtendedStoredGroup* aGroup );

        /*
         * Loads next group from database
         * @since	2.1
         * @param	aGroup	Group is read to this object
         * @return	KErrNone, if ok. KErrNotFound, if group is not found.
         */
        TInt	LoadNextL( MCAExtendedStoredGroup* aGroup );

        /*
         * Does free diskspace check before writing to database
         * @since	2.1
         * @param	aGroup group that is written to database
         * Function leaves with KErrDiskFull, if freespace goes below critical level
         */
        void	CheckFreespaceLevelL( MCAExtendedStoredGroup* aGroup );

    protected:

        /**
         * C++ default constructor.
         */
        CCAStorage();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void		ConstructL();

    private:

        /**
         * Check if database exists
         * @since	2.1
         * @return	ETrue, if database exists.
         */
        TBool		DbExists();

        /**
         * Creates new CPermanentFileStore stream and creates database on it.
         * @since	2.1
         */
        void		CreateDbL();

        /**
         * Open existing database.
         * @since	2.1
         */
        void		OpenDbL();

        /**
         * Close database.
         * @since	2.1
         */
        void		CloseDb();

        /*
         * Create tables to database
         * @since	2.1
         */
        void		CreateTablesL();

        /**
         * Add a column to colset
         * @since	2.1
         * @param	aName Name of column
         * @param	aType Type of column
         * @param	aColset Colset, where column is added.
         * @param   aMaxLength Maximum column length
         *          0 = use columns default value
         */
        void	AddColumnL( const TDesC& aName, TDbColType aType,
                         CDbColSet* aColset, TInt aMaxLength = 0 );

        /*
         * Leaves if free diskspace is low.
         * @since	2.1
         * @param	aSize amount of free diskspace needed.
         */
        void	DoFreespaceLevelCheckL( TInt aSize );

    private:

        // storage for groups, owned
        CCAGroupStorage*		iGroupStorage;


        CPermanentFileStore*	iFileStore;

        RFs		iFs;

        TStreamId				iStreamId;
        RDbStoreDatabase		iDb;
    };

#endif // CCASTORAGE_H

//  End of File
