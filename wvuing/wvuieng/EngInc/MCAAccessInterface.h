/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface to handle chat accessing.
*
*/


#ifndef MCAACCESSINTERFACE_H
#define MCAACCESSINTERFACE_H

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Observer interface to handle messages container changes.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAAccessInterface
    {
    public: // Definitions

    public: // Interface

        /**
         * @see MCAAccessInterface
         */
        virtual void LoginL(   TImpsCspIdentifier& aCspId,
                               const TDesC& aPsw,
                               const TDesC& aClientId,
                               TUint32 aAP,
                               const TDesC* aKey1 = NULL,
                               const TDesC* aKey2 = NULL ) = 0;

        /**
         * @see MCAAccessInterface
         */
        virtual void LogoutL() = 0;

    protected: // For protection.

        /**
         * Destructor
         */
        virtual ~MCAAccessInterface() {}
    };

#endif      // MCAACCESSINTERFACE_H

// End of File
