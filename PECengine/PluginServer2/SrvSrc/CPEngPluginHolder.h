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
* Description:  See class description below.
*
*/


#ifndef __CPENGPLUGINHOLDER_H__
#define __CPENGPLUGINHOLDER_H__

//  INCLUDES
#include <e32def.h>

class CPEngPluginInterface2;

//  CLASS DEFINITION
/**
 *
 *
 *
 *
 * @lib
 * @since 3.0
 */
class CPEngPluginHolder : public CBase
    {
    public:		// Constructors and destructors

        static CPEngPluginHolder* NewL();	// Static constructor
        static CPEngPluginHolder* NewLC(); // Static constructor
        virtual ~CPEngPluginHolder();				// Destructor (virtual)

    public:	// New methods

        /**
         * Returns the uid of the held plugin.
         *
         * @since 3.0
         * @return The uid
         */
        TUid Uid() const;

        /**
         * Check if the plugin is currently loaded.
         *
         * @since 3.0
         * @return ETrue if the plugin is loaded.
         */
        TBool Loaded() const;

        void LoadPluginL( TUid aPluginUid );

        const CPEngPluginInterface2* Interface() const;

    protected:	// Constructors and destructors

        CPEngPluginHolder();// Default constructor, protected to allow derivation
        void ConstructL();		// Second phase construct

    private:	// Data

        //Filename of the plugin
        TUid iUid;

        //Pointer to plugin's interface, OWNED
        CPEngPluginInterface2* iPlugin;

    };

#endif      //  __CPENGPLUGINHOLDER_H__
