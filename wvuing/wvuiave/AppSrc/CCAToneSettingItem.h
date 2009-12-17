/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines Instant Messaging alert tone setting item
*
*/



#ifndef CCATONESETTINGITEM_H
#define CCATONESETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>

class CRepository;

// CLASS DECLARATION

/**
*  Defines alert tone setting item for instant message
*
*  @lib chat.lib
*  @since Series 60 3.0
*/
class CCAToneSettingItem : public CAknSettingItem
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CCAToneSettingItem( TInt aIdentifier,
                            TDes& aToneName,
                            CRepository& aCenRep,
                            TBool aBranding = EFalse );

        /**
        * Destructor.
        */
        virtual ~CCAToneSettingItem();

    public: // Functions from base classes

        /**
        * From CAknSettingItem
        */
        void StoreL();

        /**
        * From CAknSettingItem
        */
        void LoadL();

        /**
        * From CAknSettingItem
        */
        void EditItemL( TBool aCalledFromMenu );

        /**
        * From CAknSettingItem
        */
        const TDesC& SettingTextL();

        /**
        * From CAknSettingItem
        */
        void CompleteConstructionL();

    private:    // New functions

        /**
         * Gets the default audio file name
         * @param aSoundFileName After the call this will
         *                       contain the default file name
         */
        void DefaultSoundFile( TDes& aSoundFileName );

    private:    // Data

        TFileName iInternalToneFileName;

        // External filename for selected tone
        TDes& iExternalToneFileName;

        // Own: Localised text qtn_tc_none
        HBufC* iNoSoundText;

        // Own: "No_Sound.wav" file name
        HBufC* iNoSoundFileName;

        // Own: Localised text for title for tone selection
        HBufC* iTitle; /// Title for tone selection
        TPtrC iSettingText;
        TBool iBranding;

        // does not own. Access to central repository
        CRepository& iCenRep;

        // Flag is read from CR in construction
        // @see IMVariant.hrh
        // ETrue if feature EIMFeatBrandableTones is
        // variated on, EFalse otherwise.
        TBool iBrandableToneList;
    };

#endif      // CCATONESETTINGITEM_H   

// End of File
