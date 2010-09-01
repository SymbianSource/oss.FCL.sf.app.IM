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
* Description:  Single list array decorator
*  Since	   : 3.2
*
*/



#ifndef CCASINGLELISTVIEWARRAY_H
#define CCASINGLELISTVIEWARRAY_H

//  INCLUDES
#include    <e32base.h>
#include    <bamdesca.h> // MDesCArray

// FORWARD DECLARATIONS
class MCASkinVariant;
class CAknIconArray;
class CColumnListBoxData;
class CCAContactListBoxModel;
class CFont;
class CCASingleListContainer;
class CCAContactListBox;

#include "MCAMainViewArrayPC.h"
#include "TEnumsPC.h"

// CLASS DECLARATION

/**
 *  Single list-array decorator.
 *  Decorates the array for listbox
 *
 *  @lib chatng.app
 *  @since 3.2
 */
class CCASingleListViewArray : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aMainViewArray is wrapper array of all arrays on engine side
         * @param aSelectionList Is this selection list or not
         * @param aListBoxData Listbox data for setting row properties,
         *                     can be NULL.
         * @param aShowIndicators Show indicators in D-column or not.
         * @param aAutomaticRefresh Contact list synchronization mode.
         * @param aListBox Required for owndata
         */
        static CCASingleListViewArray* NewL(
            MCAMainViewArrayPC& aMainViewArray,
            TEnumsPC::TListSelectionType aSelectionList,
            CColumnListBoxData* aListboxData /*= NULL*/,
            TBool aShowIndicators /*= ETrue*/,
            TBool aAutomaticRefresh /*=ETrue*/,
            CCAContactListBox& aListBox );
        /**
         * Destructor.
         */
        virtual ~CCASingleListViewArray();


    public: // New functions

        /**
         * Loads necessary icons for this decorator
         * @param aMbmFile Bitmap file
         * @param aSkinVariant Skin variant for loading icons
         * @return Icon array. Ownership is transferred.
         */
        static CAknIconArray* LoadIconsLC( const TDesC& aMbmFile,
                                           MCASkinVariant& aSkinVariant );

        /**
         * Sets filter for contacts
         * @aFilter new filter for contacts
         */
        void SetFilterType( TEnumsPC::TFilterType  aFilter );


        /**
         * Sets automatic presence update information
         * @aAutomaticRefresh new filter for contacts
         */
        void SetAutomaticRefresh( TBool aAutomaticRefresh );

        /**
        * Gives this array a pointer to listbox model, so it can set the proper
        * fonts in proper positions in list.
        * @param aModel Pointer to listbox model
        */
        void SetContactListBoxModel( CCAContactListBoxModel* aModel );


    public: // Functions from MDesCArray

        /**
         * From MDesCArray, Returns the number of descriptor elements in
         * a descriptor array.
         * @see MDesCArray
         */
        TInt MdcaCount() const;

        /**
         * From MDesCArray, Indexes into a descriptor array.
         * @see MDesCArray
         */
        TPtrC16 MdcaPoint( TInt aIndex ) const;


    private: // new functions

        /**
         * Append formatted contact identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendContact( TPtr& aBuffer, TInt aIndex ) const;


        /**
         * Append formatted contact list identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendContactListL( TPtr& aBuffer,
                                 TInt aIndex ) const;


        /**
         * Append formatted OwnData identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendOwnData( TPtr& aBuffer, TInt aIndex ) const;

        /**
         * Append formatted All list identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendAll( TPtr& aBuffer, TInt aIndex ) const;

        /**
         * Append formatted ConversationItem identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendConversationItem( TPtr& aBuffer, TInt aIndex ) const;

        /**
         * Append formatted OpenChats list identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendOpenChatsList( TPtr& aBuffer, TInt aIndex ) const;

        /**
         * Append formatted  Invitation identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendInvitationItem( TPtr& aBuffer, TInt aIndex ) const;

        /**
         * Append formatted GroupItem identification to buffer
         * @param aBuffer Buffer where identification will be appended
         * @param index of the item in array
         */
        void AppendGroupItem( TPtr& aBuffer, TInt aIndex ) const;


    private:

        /**
         * C++ default constructor.
         * @params, refer to NewL comment
         */
        CCASingleListViewArray(
            MCAMainViewArrayPC& aMainViewArray,
            TEnumsPC::TListSelectionType aSelectionList,
            CColumnListBoxData* aListboxData,
            TBool aShowIndicators,
            TBool aAutomaticRefresh,
            CCAContactListBox& aListBox );


        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

    private:    // Data

        // wrapper array on engine side arrays
        MCAMainViewArrayPC& iMainViewArray;

        // Is this selection list or not
        TEnumsPC::TListSelectionType iSelectionList;

        // Doesn't own. Pointer to listbox data for setting row properties
        CColumnListBoxData* iListboxData;

        // Show indicators in D-column or not
        TBool iShowIndicators;

        // Show watching indicator or not
        TBool iAutomaticRefresh;

        // Owns. Buffer that hold's information about fetched item
        HBufC* iData;

        // maxmimun length of icon strings added to formatted text
        TInt iMaxLengthOfIcons;

        // owns. text for "All"
        HBufC* iAllText;

        // pointers to listbox fonts, contained objects are not owned
        RPointerArray<CFont> iFonts;

        //Filter for contacts
        //TStorageManagerGlobals::TFilterType iFilter;
        TEnumsPC::TFilterType iFilter;

        // Not own. Model to check the real location of items.
        CCAContactListBoxModel* iContactListBoxModel;

        //Ref. to Contact List Box
        CCAContactListBox& iListBox;

    };

#endif      // CCASINGLELISTVIEWARRAY_H

// End of File
