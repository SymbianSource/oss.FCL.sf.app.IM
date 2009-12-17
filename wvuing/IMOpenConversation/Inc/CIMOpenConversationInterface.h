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
* Description:  Open conversation plugin
*  Interface   : Private
*
*/


#ifndef __CIMOPENCONVERSATION_H
#define __CIMOPENCONVERSATION_H


// INCLUDES
#include <e32base.h>
#include <ECom/ECom.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;


// CLASS DECLARATION

/**
 * Plug-in interface for opening IM conversation.
 */
class CIMOpenConversationInterface : public CBase
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of CIMOpenConversationInterface.
         */
        static CIMOpenConversationInterface* NewLC();
        /**
         * Destructor.
         */
        virtual ~CIMOpenConversationInterface();

    public: // Interface
        /**
         * Opens conversation with aContact. Uses a aContact's wvid
         * for identification.
         *
         * @param aContact conversation to be opened with
         * @param aPbkEngine forwarded to search
         */
        virtual void OpenConversationL
        ( CPbkContactItem& aContact, CPbkContactEngine& aPbkEngine ) = 0;

    protected: // Implementation
        CIMOpenConversationInterface();

    private:  // Data
        TUid iDtor_ID_Key;
    };

// INLINE FUNCTIONS

inline CIMOpenConversationInterface* CIMOpenConversationInterface::NewLC()
    {
    const TUid KIMOpenConversationImplementationUID = { 0x101f8595 };

    TAny* ptr = REComSession::CreateImplementationL
                ( KIMOpenConversationImplementationUID,
                  _FOFF( CIMOpenConversationInterface, iDtor_ID_Key ) );
    CIMOpenConversationInterface* impl =
        reinterpret_cast<CIMOpenConversationInterface*>( ptr );
    CleanupStack::PushL( impl );
    return impl;
    }

inline CIMOpenConversationInterface::~CIMOpenConversationInterface()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

inline CIMOpenConversationInterface::CIMOpenConversationInterface()
    {
    }

#endif

// End of file

