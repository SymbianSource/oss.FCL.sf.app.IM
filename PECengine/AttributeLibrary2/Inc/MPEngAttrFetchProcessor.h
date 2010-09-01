/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute fetch processor interface.
*
*/

#ifndef __MPENGATTRFETCHPROCESSOR_H
#define __MPENGATTRFETCHPROCESSOR_H

//INCLUDES
#include <E32Base.h>
#include <BaDescA.h>
#include "PEngWVCspVersion.h"


// FORWARD DECLARATIONS
class MPEngAttrFetchProcessor;
class MPEngPresenceAttrManager;
class MPEngPresenceAttrModel2;
class MPEngAdvTransactionStatus2;



/**
 * Factory method to create attribute fetch processor.
 *
 * @since 3.0
 * @param aAttrManager The attribute manger to use in
 *                     attribute handling.
 * @param aFetchedPresenceIds The presence ID's to fetch.
 * @param aFetchedAttributes The presence attributes to fetch.
 * @param aDomain The user domain where the fetching happens.
 * @param aTransactionOperation The transaction operation id.
 * @param aCspVersion The CSP version to use in transaction.
 */
IMPORT_C MPEngAttrFetchProcessor* CreateAttrFetchProcessorL(
    MPEngPresenceAttrManager& aAttrManager,
    const MDesCArray& aFetchedPresenceIds,
    const TArray< TUint32 > aFetchedAttributes,
    const TDesC& aDomain,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion );



//  CLASS DEFINITION

/**
 * Attribute fetch processor interface.
 *
 * @since 3.0
 */
class MPEngAttrFetchProcessor
    {
    public:  //Destructor

        /**
         * Destructor.
         *
         * @since 3.0
         */
        virtual ~MPEngAttrFetchProcessor() {};


    public: // Pure virtual functions

        /**
         * Method to generate fetch request.
         *
         * Generates attribute fetch request based on
         * attribute types and presence ID's given in
         * concrete object instantiation.
         *
         * @since 3.0
         * @param aSendBuffer The send buffer where to store the
         * request. Leaves if the request doesn't fit in the buffer.
         */
        virtual void GenerateRequestL( TDes8& aSendBuffer ) = 0;


        /**
         * Initializes the fetch response processing.
         * Further processing steps are executed
         * by calling ProcessStepL() function repeatedly
         * until it returns EFalse.
         *
         * If passed response isn't valid response
         * to Get-Presence request, leaves with KErrCorrupt.
         *
         * If network server returns errors in response
         * message, those are available to client from
         * transaction result.
         *
         * @since 3.0
         * @param aResponse The XML response from the
         *        network presence server.
         * @param aResultModels Array where to append extracted
         *        attribute models. Array is used along each result
         *        processing step and it should stay valid until
         *        all steps are processed. Array ownership remains
         *        on the client. Also generated attribute models
         *        ownership is returned to client.
         * @param aResultStatus Transaction status container where
         *        to append response status. Status container is used
         *        along each result processing step and it should stay
         *        valid until all steps are processed through.
         *        Status container ownership remains on the client.
         */
        virtual void InitResponseProcessingL(
            const TDesC8& aResponse,
            RPointerArray< MPEngPresenceAttrModel2 >& aResultModels,
            MPEngAdvTransactionStatus2& aResultStatus ) = 0;


        /**
         * Processes the next part from fetch response.
         *
         * @since 3.0
         * @return ETrue if there is further processing steps left .
         *         EFalse if no more further processing steps needed.
         */
        virtual TBool ProcessStepL() = 0;



    };

#endif      // __MPENGATTRFETCHPROCESSOR_H

//  End of File

