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
* Description:  Setting item for message colour
*
*/


#ifndef C_COLOURSETTINGITEM_H
#define C_COLOURSETTINGITEM_H

#include <aknsettingitemlist.h>

class MCASettingSapExt;

/**
 *  Colour setting item.
 *
 *  Custom setting item to be used with colour selections.
 *  Setting value buffer (TBool& aBuffer in constructor)
 *  tells whether default value is selected (EFalse) or
 *  user defined colour is used (ETrue). When the default
 *  value is used setting item shows text qtn_chat_set_colour_default.
 *  When user defined colour is used the setting items
 *  value box is coloured with that colour. EditItemL
 *  method is overridden to launch a colour selection grid.
 *
 *
 *  @lib chat.exe
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCAColourSettingItem ) : public CAknSettingItem
    {
public:

    /**
     * Constructor.
     * @param aSAPExtension Reference to SAP settings.
     * @param aBuffer Reference to external settings value buffer.
     * @param aIdentifier Unique identifier for setting item.
     */
    CCAColourSettingItem(
        MCASettingSapExt& aSAPExtension,
        TBool& aBuffer,
        TInt aIdentifier );

    /**
     * Symbian 2nd phase constructor.
     *
     * NOTE! Use this instead of ConstructL from CAknSettingItem.
     *       If this is not used CompleteConstructionL() panics!
     *
     * @param aKey Key for key-value pair in SAP settings,
     *             used to get colour information from SAP settings.
     * @see CAknSettingItem for rest of parameters.
     */
    void ConstructL(
        const TDesC& aKey,
        const TInt aOrdinal,
        const TDesC& aSettingTitle,
        CArrayPtr<CGulIcon>* aIconArray );

    virtual ~CCAColourSettingItem();

// from base class CAknSettingItem

    /**
     * From CAknSettingItem.
     * @see CAknSettingItem
     *
     * @since S60 v3.2
     */
    void StoreL();

    /**
     * From CAknSettingItem.
     * @see CAknSettingItem
     *
     * @since S60 v3.2
     */
    void LoadL();

    /**
     * From CAknSettingItem.
     * @see CAknSettingItem
     *
     * @since S60 v3.2
     */
    void EditItemL( TBool aCalledFromMenu );

    /**
     * From CAknSettingItem.
     * @see CAknSettingItem
     *
     * @since S60 v3.2
     */
    const TDesC& SettingTextL();

protected:

// from base class CAknSettingItem

    /**
     * From CAknSettingItem.
     * @see CAknSettingItem
     *
     * @since S60 v3.2
     */
    void CompleteConstructionL();

    /**
     * From CAknSettingItem.
     * @see CAknSettingItem
     *
     * @since S60 v3.2
     */
    CFbsBitmap* CreateBitmapL();

private:

    /**
     * This sets the mask of this setting item's icon array, if it exists,
     * using the mask from the passed icon.
     * It returns the bitmap from the aIcon.
     * The method also takes ownership of aIcon and
     * may delete it (but not its bitmap).
     *
     * @since S60 v3.2
     * @param aIcon Icon of which the mask is passed through to SetIconMask()
     *              and bitmap is returned.
     * @return Bitmap from aIcon. Ownership of the bitmap is
     *         transfered to the caller.
     */
    CFbsBitmap* SetIconMaskAndReturnBitmap( CGulIcon* aIcon );

private: // data

    /**
     * Reference to external value of setting.
     */
    TBool& iExternalValue;

    /**
     * Internal setting value.
     */
    TBool iInternalValue;

    /**
     * Text used when default option selected.
     * Own.
     */
    HBufC* iDefaultText;

    /**
     * Key for key-value pair in SAP settings.
     * Own.
     */
    HBufC* iSAPKey;

    /**
     * Reference to SAP settings.
     * Not own.
     */
    MCASettingSapExt& iSAPExtension;

    };

#endif // C_COLOURSETTINGITEM_H
