/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Reactive authorization responds implementation.
*
*/

#ifndef CPENGAUTHORIZATIONRESPOND_H__
#define CPENGAUTHORIZATIONRESPOND_H__

//  INCLUDES
#include <E32Base.h>
#include "MPEngAuthorizationRespond.h"


//FORWARD DECLARATIONS
class MPEngAuthorizationManager;


// CLASS DECLARATION
/**
 * Reactive authorization responds implementation.
 *
 * @since 3.0
 */
class CPEngAuthorizationRespond : public CBase,
            public MPEngAuthorizationRespond
    {
    public:  // Constructors and destructor

        /**
         *  Two-phased constructor,
         */
        static CPEngAuthorizationRespond * NewLC();


        /**
         *  Destructor.
         */
        virtual ~CPEngAuthorizationRespond();


    protected: // constructors

        /**
         * C++ default constructor.
         */
        CPEngAuthorizationRespond();

    public: // New functions



        /**
         *
         *
         *
         */
        void LockRespondToAuthManagerL( MPEngAuthorizationManager& aAuthMan );



    protected: // Functions from MPEngAuthorizationRespond

        /**
         * Authorization responds extension interface.
         * @see MPEngAuthorizationRespond.h
         * @since 3.0
         */
        MPEngAuthorizationRespondExt* Extension();
        const MPEngAuthorizationRespondExt* Extension() const;


        /**
         * Authorization responds internal extension interface.
         * @see MPEngAuthorizationRespond.h
         * @since 3.0
         */
        const MPEngAuthorizationRespondAdvanced* Advanced() const;
        MPEngAuthorizationRespondAdvanced* Advanced();


        /**
         * Authorization responds cleanup support.
         * @see MPEngAuthorizationRespond.h
         * @since 3.0
         */
        void Close();



    private:    // Data

        //OWN: Authorization manager
        MPEngAuthorizationManager*  iAuthManager;

    };

#endif      // CPENGAUTHORIZATIONRESPOND_H__

// End of File

