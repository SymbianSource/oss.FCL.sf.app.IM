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
* Description:  Eikon application architectures document class
*
*/



#ifndef CCADOCUMENT_H
#define CCADOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CEikAppUi;
class CCAEngine;
class MCAProcessManager;

// CLASS DECLARATION

/**
*  Eikon application architectures document class
*
*  @lib chat.app
*  @since 1.2
*/
class CCADocument : public CAknDocument

    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aApp Handle to Eikon's application class
        */
        static CCADocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CCADocument();

    private:
        /**
        * Default constructor
        * @param aApp Handle to Eikon's application class
        */
        CCADocument( CEikApplication& aApp ): CAknDocument( aApp ) { }

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CEikDocument, create CChatClientAppUi "App UI" object.
        * @see CEikDocument::CreateAppUiL
        */
        CEikAppUi* CreateAppUiL();

    private:
        // Owns. Application model (engine)
        CCAEngine* iEngine;
        //owns, Appln's Process Manager
        MCAProcessManager* iProcessManager;
    };

#endif // CCADOCUMENT_H

// End of File

