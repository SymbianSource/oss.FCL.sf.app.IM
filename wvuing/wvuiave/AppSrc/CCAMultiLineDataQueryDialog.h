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
* Description:  Multiline dataquery
*
*/


#ifndef CCAMULTILINEDATAQUERYDIALOG_H
#define CCAMULTILINEDATAQUERYDIALOG_H

//  INCLUDES
#include <aknquerydialog.h>


// CLASS DECLARATION

/**
*  Query Dialog with data input on more than one line (2 lines at the moment)
*  Updates left softkey if first or second line has data
*
*  @lib chatng.app
*/
class CCAMultiLineDataQueryDialog : public CAknMultiLineDataQueryDialog
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aText1 First line text
        * @param aText2 Second line
        * @param aTone Tone
        * @return An instance of this class
        */
        static CCAMultiLineDataQueryDialog* NewL( TDes&  aText1,
                                                  TDes&  aText2/*,
												  TTone aTone = ENoTone*/ );

        /**
        * Destructor.
        */

        virtual ~CCAMultiLineDataQueryDialog();


    protected:  // Functions from base classes

        /**
        * From CAknMultiLineDataqueryDialog
        * If the query text is ok (retrieved form query controls)
        * then display the left soft key, otherwise hide it.
        * @see CAknMultiLineDataqueryDialog
        */
        void UpdateLeftSoftKeyL();


    private:

        /**
        * C++ default constructor.
        */
        CCAMultiLineDataQueryDialog();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };

#endif      // CCAMULTILINEDATAQUERY_H

// End of File
