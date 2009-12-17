/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for Presence Engine Plugins.
*
*/

#ifndef __CPENGPLUGININTERFACE2_H
#define __CPENGPLUGININTERFACE2_H

// INCLUDES
#include <e32base.h>
#include <ECom/ECom.h>


// CONSTANTS
const TUid KPEngPluginInterfaceUid = { 0x101FB0E6 };



// CLASS DECLARATION

/**
 * ECom interface for Presence Engine Plugins.
 *
 */
class CPEngPluginInterface2 : public CBase
    {
    public: // Constructor and destructor
        /**
         * Constructor.
         * @param aImplementationUid ECom implementation UID of the plugin.
         */
        static CPEngPluginInterface2* NewL( TUid aImplementationUid );


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngPluginInterface2();



    protected: // Implementation

        CPEngPluginInterface2();



    private:  // Data

        //Own: destructor id key
        TUid iDtor_ID_Key;
    };

#include "CPEngPluginInterface2.inl"

#endif // __CPENGPLUGININTERFACE2_H


// End of File



