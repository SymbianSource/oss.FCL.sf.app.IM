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
* Description:  Interface class for variation factory. Variation factory
*				 returns different UI features depending on software version.
*
*/



#ifndef CCAVARIANTFACTORY_H
#define CCAVARIANTFACTORY_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MCASkinVariant;
class MCAOpBrandVariant;
class MCAAHVariant;

// CLASS DECLARATION

/**
*  Interface class for variation factory
*
*  @lib chat.app
*  @since 1.2s
*/
class CCAVariantFactory : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAVariantFactory* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAVariantFactory();

    public: // New functions

        /**
        * Returns skin variant.
        * @since 1.2
        * @return MCASkinVariant*, skin variant object.
        */
        virtual MCASkinVariant*	SkinVariantL() = 0;
        /**
        * Returns resource file/bitmap alternating variant.
        * @since 1.2
        * @return MCAOpBrandVariant*, variant object.
        */
        virtual MCAOpBrandVariant*	OpBrandVariantL() = 0;
        /**
        * Returns Arabic/Hebrew variant.
        * @since 1.2
        * @return MCAAHVariant*, variant object.
        */
        virtual MCAAHVariant*		AHVariantL() = 0;


    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CCAVariantFactory();
    };

#endif      // CCAVARIANTFACTORY_H

// End of File
