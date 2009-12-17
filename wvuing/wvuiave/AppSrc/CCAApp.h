/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Uikon application architecture's application class
 *
*/



#ifndef CCAAPP_H
#define CCAAPP_H

// INCLUDES
#include <aknapp.h>

// FORWARD DECLARATIONS
class CCAVariantFactory;

// CLASS DECLARATION

/**
 *  Application architecture's application
 *
 *  @lib chat.app
 *  @since 1.2
 */
class CCAApp : public CAknApplication
    {

    public: // Constructors and destructors

        virtual ~CCAApp();

    public: // New methods

        /**
         * Returns pointer to variant factory
         * @return A pointer to the created document object.
         */
        CCAVariantFactory* VariantFactory() const;

    private:

        /**
         * From CApaApplication, creates CChatClientDocument document object.
         * @return A pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /**
         * From CApaApplication, returns application's UID (KUidChatClient).
         * @return The value of KUidChatClient
         */
        TUid AppDllUid() const;

    private: // Data

        //Owns
        CCAVariantFactory* iVariantFactory;
    };

#endif // CCAAPP_H

// End of File
