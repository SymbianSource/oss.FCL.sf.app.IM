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
* Description:  Represents the search data PC Interface for UI
*
*/


#ifndef MCASEARCHDATAPC_H
#define MCASEARCHDATAPC_H


// INCLUDES
#include	"MCASearchData.h" //from caengine.dll


//PLATFORM INCLUDES
#include <e32std.h>



//INTERFACE DECLARATION
/**
*  Search data representation for the UI.
*
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class MCASearchDataPC
    {
    public:

        /**
        * Returns searched data.
        * @param aIndex specifies the index to the wanted data.
        * @return This is the search data.
        */
        virtual TPtrC SearchData( TInt aIndex ) const = 0;

        /**
        * Returns the current search data count
        * ( I.e. maximum number of available items ).
        * @return Count of data items.
        */
        virtual TInt SearchDataCount() const = 0;

        /**
        * The maximum amount of results available in the server.
        * @return The amount.
        */
        virtual TInt HowManyResultsAvailable() const = 0;

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
        virtual const TPtrC IdentificationL( const TDesC& aContactId ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCASearchDataPC() {};
    };

#endif      // MCASEARCHDATAPC_H

// End of File
