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
* Description:  This class declares info array for chat info. This is
 *				  customized for using multiple lines.
 *				  (Item texts are not truncated)
 *
*/



#ifndef CCACHATINFOARRAY_H
#define CCACHATINFOARRAY_H

//  INCLUDES
#include <bamdesca.h> //MDesCArray
#include <badesca.h>

// FORWARD DECLARATIONS
class CFont;
// CLASS DECLARATION

/**
 *  Declares chat info array
 *
 *  @lib chat.app
 *  @since 1.2
 */
class CCAChatInfoArray : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CCAChatInfoArray* NewL();

        /**
         * Destructor.
         */
        virtual ~CCAChatInfoArray();

    public: // New functions

        /**
         * This method sets the item label and fieldtext for items
         * @since 1.2
         * @param aLabelResourceId Resource id for label (header)
         * @param aFieldText Descriptor that contains field text
         * @param aWrap ETrue if items are wrapped
         */
        void AddLabelAndTextL( TInt aLabelResourceId, const TDesC& aFieldText,
                               TBool aWrap = ETrue );

    private: //New functions

        /**
         * This method sets the line width array for AknTextUtils::
         * WrapToArrayL()
         * @since 1.2
         * @param aLineWidthArray Array that contains line widths
         * @param aTextField Descriptor that contains field text.
         *		  With this descriptor the number of lines are declared
         */
        void SetLineWidthArrayL( CArrayFix<TInt>* aLineWidthArray,
                                 const TDesC& aTextField );


    public: // from MDesCArray

        /**
         * From MDesCArray Returns the number of descriptor elements
         * in a descriptor array
         * @return The number of descriptor elements in a descriptor array
         */
        TInt MdcaCount() const;

        /**
         * From MDesCArray Indexes into a descriptor array
         * @param aIndex The position of the descriptor element within a
         *		  descriptor array. The position is relative to zero; i.e. zero
         *        implies the first descriptor element in a descriptor array.
         * @return A 16 bit non-modifiable pointer descriptor representing the
         *		   descriptor element located at position aIndex within a
         *		   descriptor array.
         */
        TPtrC MdcaPoint( TInt aIndex ) const;


    private:

        /**
         * Default constructor
         */
        CCAChatInfoArray();

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

    private:
        //owns
        CDesCArray* iTextFields;

        //owns
        CArrayFix<TInt>* iLineWidthArray;

        //owns
        HBufC* iLine;

        //doesn't own
        const CFont* iFont;

        TInt iSubCellWidth;
    };

#endif      // CCACHATINFOARRAY_H

// End of File
