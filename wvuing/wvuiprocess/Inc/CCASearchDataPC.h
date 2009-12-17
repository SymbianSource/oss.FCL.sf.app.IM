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
* Description:  Represents the search data for UI
*
*/


#ifndef CCASEARCHDATAPC_H
#define CCASEARCHDATAPC_H


// INCLUDES
#include	"MCASearchDataPC.h"


//CLASS DECLARATION
/**
*  Search data representation for the UI.
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class CCASearchDataPC: public CBase,
            public MCASearchDataPC
    {

    public:

        /**
         * Symbian Two Phase Constructor
         * @param aSearchData : a reference to searchdata interface in the chat engine
         */
        static CCASearchDataPC* NewL( MCASearchData* aSearchData );


        /**
         * virtual destructor
         */
        virtual ~CCASearchDataPC();

    private:

        /**
         * default constructor
         * @param aSearchData : a reference to searchdata interface in the chat engine
         */

        CCASearchDataPC( MCASearchData* aSearchData );


    public:

        /**
         * Returns searched data.
         * @param aIndex specifies the index to the wanted data.
         * @return This is the search data.
         */
        TPtrC SearchData( TInt aIndex ) const ;

        /**
        * Returns the current search data count
        * ( I.e. maximum number of available items ).
        * @return Count of data items.
        */
        TInt SearchDataCount() const ;

        /**
        * The maximum amount of results available in the server.
        * @return The amount.
        */
        TInt HowManyResultsAvailable() const ;

        /**
         * Identification of contact. This is resolved from known
         * identifications.
         * Alias, nickname and contactId.
         * Nickname is shown if set.
         * @param aContactId Id of contact which identification needed.
         * @return Identification of contact.
         *      Alias if nickname is not set and alias is enabled.
         *      Wvid without domain and prefix if nickname does not
                exists and alias is not enabled or not exists.
         */
        const TPtrC IdentificationL( const TDesC& aContactId ) ;

    private:

        /**
        * iDataInterface: a reference to searchdata interface in the chat engine
        * Doesnt Own
        */
        MCASearchData* iDataInterface;

    };

#endif      // CCASEARCHDATAPC_H

// End of File
