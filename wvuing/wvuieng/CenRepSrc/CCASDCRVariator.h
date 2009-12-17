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
* Description:  SD and CR variation
*
*/


#ifndef CCASDCRVARIATOR_H
#define CCASDCRVARIATOR_H

//  INCLUDES
#include <e32base.h>
#include "MCASDCRVariator.h"

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
 *  Persistent preference store implementation for
 *  Central Repository
 *  @lib CAEngine.lib
 *  @since 3.0
 */
class CCASDCRVariator : public CBase, public MCASDCRVariator
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor
         * @param aIdentifier The UID of the application, ignored in cenrep
         */
        static CCASDCRVariator* NewL( TUid aIdentifier );

        /**
         * Destructor.
         */
        virtual ~CCASDCRVariator();

    public: // MCASDCRVariator

        /**
         * @see MCASDCRVariator
         */
        TInt GetInt( TInt aSetting, TInt& aValue );

        /**
         * @see MCASDCRVariator
         */
        TInt SetInt( TInt aSetting, TInt aValue );

        /**
         * @see MCASDCRVariator
         */
        TInt GetString( TInt aSetting, TDes& aValue );

        /**
         * @see MCASDCRVariator
         */
        TInt SetString( TInt aSetting, const TDesC& aValue );

        /**
         * @see MCASDCRVariator
         */
        TInt FlushData();

    private: // New methods

        /**
         * C++ default constructor.
         */
        CCASDCRVariator();

        /**
         * By default Symbian OS constructor is private.
         * @see CCASDCRVariator::NewL.
         */
        void ConstructL( TUid aIdentifier );

        /**
         * Central repository:
         * Generate key value from a given setting number.
         * @param aSettingNumber Number (enum) of setting
         * @return Generated key
         * @see MCASettings for the setting enumerations
         * @since 3.0
         */
        TInt GenerateKey( const TInt aSettingNumber ) const;

    private:

        // central repository
        CRepository* iCenRep;   // private and internal keys, owns
    };

#endif      // CCASDCRVARIATOR_H

// End of File
