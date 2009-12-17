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
* Description:  Attribute fetch processor.
*
*/

#ifndef CPENGFETCHPROCESSOR_H__
#define CPENGFETCHPROCESSOR_H__


//  INCLUDES
#include <E32base.h>
#include <BaDescA.h>
#include "MPEngAttrFetchProcessor.h"
#include "PEngWVCspVersion.h"



//FORWARD DECLARATION
class CPEngAttributeTypeStack;
class MPEngAdvTransactionStatus2;
class MPEngPresenceAttrManager;
class MPEngPresenceAttrModel2;
class MPEngXMLParser;
class MPEngXMLSerializer;




// CLASS DECLARATION

/**
 * Processor to handle presence attribute fetch XML request
 * generation and response decoding.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngFetchProcessor ) : public CBase,
        public MPEngAttrFetchProcessor
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aAttrManager The attribute manger to use in
     * attribute handling.
     * @param aFetchedPresenceIds The presence ID's to fetch.
     * @param aFetchedAttributes The presence attributes to fetch.
     * @param aDomain The user domain where the fetching happens.
     * @param aTransactionOperation The transaction operation id.
     * @param aCspVersion The CSP version for request.
     */
    static CPEngFetchProcessor* NewL( MPEngPresenceAttrManager& aAttrManager,
    const MDesCArray& aFetchedPresenceIds,
    const TArray< TUint32 > aFetchedAttributes,
    const TDesC& aDomain,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion );


    /**
     * Destructor.
     */
    virtual ~CPEngFetchProcessor();




private: //Constructors

    /**
     * C++ constructor.
     */
    CPEngFetchProcessor( MPEngPresenceAttrManager& aAttrManager,
                         TInt aTransactionOperation,
                         TPEngWVCspVersion aCspVersion );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL( const MDesCArray& aFetchedPresenceIds,
                     const TArray< TUint32 > aFetchedAttributes,
                     const TDesC& aDomain );



public: //From MPEngAttrFetchProcessor


    /**
     * @see MPEngAttrFetchProcessor
     */
    void GenerateRequestL( TDes8& aSendBuffer );


    /**
     * @see MPEngAttrFetchProcessor
     */
    void InitResponseProcessingL( const TDesC8& aResponse,
                                  RPointerArray< MPEngPresenceAttrModel2 >& aResultModels,
                                  MPEngAdvTransactionStatus2& aResultStatus );

    /**
     * @see MPEngAttrFetchProcessor
     */
    TBool ProcessStepL();



private: //Request helpers


    /**
     * Request generation helper.
     *
     * @since 3.0
     * @param aSerializer The serializer to write request.
     * @param aFetchedPresenceIds List of fetched presence ids.
     * @param aFechedAttributeTypes Fetched attributes.
     * @param aCspVersion The CSP version for request.
     */
    static void GenerateRequestL(
        MPEngXMLSerializer& aSerializer,
        const CDesC16ArraySeg& aFetchedPresenceIds,
        const CPEngAttributeTypeStack& aFechedAttributeTypes,
        TPEngWVCspVersion aCspVersion );



    /**
     * Parsing helper.
     *
     * Checks the transaction status from network response
     * Handles also partial failure, by removing the unknown presence
     * id's from the list of requested presence ids. In case of complete
     * failure, adds all requested presence id's as detailed results
     * to transaction status.
     *
     * @since 3.0
     * @param aResponse Get presence response to check.
     * @param aDomain The domain where fetch takes its place.
     * @param aTransactionOperation The transaction operation id.
     * @param aParser XML parser to use in parsing.
     * @param aParsedModels Array where to append the parsed models.
     * @param aTransactionStatus The transaction status container to fill.
     * @param aRequestedPresenceIds The list of requested presence IDs.
     * @return Transaction continue status. ETrue if response should
     * be parsed further. EFalse if no need for further processing.
     */
    static TBool CheckResultFromFromResponseL(
        const TDesC8& aResponse,
        const TDesC& aDomain,
        TInt aTransactionOperation,
        MPEngXMLParser& aParser,
        MPEngAdvTransactionStatus2& aTransactionStatus,
        CDesC16Array& aRequestedPresenceIds );


    /**
     * Parsing helper.
     *
     * Parses presence blocks from complete Get-Presence-Response.
     * Appends found presence blocks to given array.
     *
     * @since 3.0
     * @param aResponse Get presence response to parse.
     * @param aParser XML parser to use in parsing.
     * @paramaPresenceBlocks Array to append the parsed blocks.
     */
    static void LocatePresenceBlocksFromResponseL(
        const TDesC8& aResponse,
        MPEngXMLParser& aParser,
        CDesC8Array& aPresenceBlocks );


    /**
     * Parses attribute models from presence block
     * (XML fragment divided by <Presence> tags).
     *
     * Parsing produces only those attributes models which
     * are listed in given aAttributeTypes and which are
     * found from network response.
     *
     * @since 3.0
     * @param aPresenceBlock One presence id's presence XML fragment.
     * @param aAttrManager The attribute manager to use in model construction.
     * @param aParser XML parser to use in parsing.
     * @param aFetchedPresenceIds ID's which are fetched.
     * Handled presence ID's are removed from the array.
     * @param aDomain The domain where fetch takes its place.
     * @param aAttributeTypes Ordered attribute types.
     * @param aResultModels Array where to append the parsed result models.
     */
    static void ParseModelsFromBlockL(
        const TDesC8& aPresenceBlock,
        MPEngPresenceAttrManager& aAttrManager,
        MPEngXMLParser& aParser,
        CDesC16Array& aFetchedPresenceIds,
        const TDesC& aDomain,
        const CPEngAttributeTypeStack& aAttributeTypes,
        RPointerArray<MPEngPresenceAttrModel2>& aResultModels,
        TInt aOperation );


private:    // Data


    //REF: Attribute Manager
    MPEngPresenceAttrManager&   iAttributeManager;

    //OWN: Operation id
    const TInt  iTransactionOperation;

    //OWN: WV CSP version
    const TPEngWVCspVersion  iCspVersion;

    //OWN: Array of fetched presence ID's
    CDesC16ArraySeg     iFetchedPresenceIds;

    //OWN: Attribute type stack for fetched attributes
    CPEngAttributeTypeStack*    iFechedAttributeTypes;

    //OWN: XML parser
    MPEngXMLParser*     iXmlParser;

    //OWN: Operation domain
    HBufC*  iDomain;

    //OWN: Presence blocks.
    CDesC8ArraySeg     iPresenceBlocks;

    //REF: Decoded attribute models
    RPointerArray< MPEngPresenceAttrModel2 >*    iFetchedModels;

    //REF: Fetcher transaction status
    MPEngAdvTransactionStatus2*     iTransactionStatus;

    };

#endif      //  CPENGFETCHPROCESSOR_H__



//  End of File

