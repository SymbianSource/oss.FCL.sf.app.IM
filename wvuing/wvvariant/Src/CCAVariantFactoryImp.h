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
* Description:  Concrete implementation of variation factory class.
*
*/



#ifndef CCAVARIANTFACTORYIMP_H
#define CCAVARIANTFACTORYIMP_H

//  INCLUDES
#include "CCAVariantFactoryImp.h"
#include "CCAVariantFactory.h"
#include "CCASkinVariant20.h"
#include "CCAOpBrandVariant21.h"
#include "CCAAHVariant20.h"
#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class MCASkinVariant;

// CLASS DECLARATION

/**
*  Concrete implementation of variation factory class.
*  See comments from base class CCAVariantFactory.h.
*
*  @lib chatng.app
*  @since 1.2s
*/
class CCAVariantFactoryImp : public CCAVariantFactory
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAVariantFactoryImp* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAVariantFactoryImp();


    public: // Functions from base classes

        /**
        * From CCAVariantFactory
        * @since 1.2s
        * See method comments from CCAVariantFactory.h.
        */
        MCASkinVariant*	SkinVariantL();
        MCAOpBrandVariant*	OpBrandVariantL();
        MCAAHVariant*		AHVariantL();


    private:

        /**
        * C++ default constructor.
        */
        CCAVariantFactoryImp();

    private:    // Data

        // active skin variant
        CCASkinVariant20* iSkinVariant;
        // active resource file&bitmap changing variant
        CCAOpBrandVariant21* iOpBrandVariant;
        // active A/H variant
        CCAAHVariant20* iAHVariant;

    };

#endif      // CCAVARIANTFACTORYIMP_H

// End of File
