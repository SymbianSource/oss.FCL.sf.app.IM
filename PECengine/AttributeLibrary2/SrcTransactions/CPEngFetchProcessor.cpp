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

// INCLUDE FILES
#include "CPEngFetchProcessor.h"
#include "CPEngAttributeTypeStack.h"
#include "CPEngTransactionStatus.h"
#include "PEngAddressUtils.h"
#include "MPEngPresenceAttrManager.h"
#include "MPEngPresenceAdvancedAttrModel2.h"

#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"
#include "PEngWVPresenceAttributes2.h"
#include "PEngAttrModelHelper.h"

#include "PresenceDebugPrint.h"

#include <PEngWVPresenceErrors2.h>
#include <E32Std.h>




// CONSTANTS
/**
 * Granularity for array of fetched presence ids.
 * Usually there is just few fetched presence id's
 * at time.
 */
const TInt KPEngFetchPresIdGranurality = 5;



// ========================== EXPORTED FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CreateAttrFetchProcessorL()
// -----------------------------------------------------------------------------
//
EXPORT_C MPEngAttrFetchProcessor* CreateAttrFetchProcessorL(
    MPEngPresenceAttrManager& aAttrManager,
    const MDesCArray& aFetchedPresenceIds,
    const TArray< TUint32 > aFetchedAttributes,
    const TDesC& aDomain,
    TInt aTransactionOperation,
    TPEngWVCspVersion aCspVersion )
    {
    return CPEngFetchProcessor::NewL( aAttrManager,
                                      aFetchedPresenceIds,
                                      aFetchedAttributes,
                                      aDomain,
                                      aTransactionOperation,
                                      aCspVersion );
    }




// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPEngFetchProcessor::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngFetchProcessor* CPEngFetchProcessor::NewL( MPEngPresenceAttrManager& aAttrManager,
                                                const MDesCArray& aFetchedPresenceIds,
                                                const TArray< TUint32 > aFetchedAttributes,
                                                const TDesC& aDomain,
                                                TInt aTransactionOperation,
                                                TPEngWVCspVersion aCspVersion )
    {
    CPEngFetchProcessor* self = new ( ELeave ) CPEngFetchProcessor(
        aAttrManager,
        aTransactionOperation,
        aCspVersion );

    CleanupStack::PushL( self );
    self->ConstructL( aFetchedPresenceIds, aFetchedAttributes, aDomain );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CPEngFetchProcessor::~CPEngFetchProcessor()
    {
    iFetchedPresenceIds.Reset();

    delete iFechedAttributeTypes;
    delete iDomain;
    delete iXmlParser;

    iPresenceBlocks.Reset();
    }


// -----------------------------------------------------------------------------
// CPEngFetchProcessor::CPEngFetchProcessor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEngFetchProcessor::CPEngFetchProcessor( MPEngPresenceAttrManager& aAttrManager,
                                          TInt aTransactionOperation,
                                          TPEngWVCspVersion aCspVersion )
        : iAttributeManager( aAttrManager ),
        iTransactionOperation( aTransactionOperation ),
        iCspVersion( aCspVersion ),
        iFetchedPresenceIds( KPEngFetchPresIdGranurality ),
        iPresenceBlocks( KPEngFetchPresIdGranurality )
    {
    }


// -----------------------------------------------------------------------------
// CPEngFetchProcessor::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEngFetchProcessor::ConstructL( const MDesCArray& aFetchedPresenceIds,
                                      const TArray< TUint32 > aFetchedAttributes,
                                      const TDesC& aDomain )
    {
    __ASSERT_ALWAYS( aFetchedPresenceIds.MdcaCount() > 0, User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aFetchedAttributes.Count() > 0, User::Leave( KErrArgument ) );

    iXmlParser = CreateXMLParserL();
    iDomain = aDomain.AllocL();

    const TInt presenceIdCount = aFetchedPresenceIds.MdcaCount();
    for ( TInt ix = 0; ix < presenceIdCount; ix++ )
        {
        //Allow one presence ID only once
        iFetchedPresenceIds.InsertIsqL( aFetchedPresenceIds.MdcaPoint( ix ),
                                        ECmpNormal );
        }


    //Gather attribute names & name spaces
    //This will also validate that attributes are valid network attributes
    iFechedAttributeTypes = CPEngAttributeTypeStack::NewL();
    iFechedAttributeTypes->PushAttributesL( aFetchedAttributes,
                                            iAttributeManager );
    }


// -----------------------------------------------------------------------------
// CPEngFetchProcessor::GenerateRequestL()
// -----------------------------------------------------------------------------
//
void CPEngFetchProcessor::GenerateRequestL( TDes8& aSendBuffer )
    {
    aSendBuffer.Zero();
    MPEngXMLSerializer* serialize = CreateXmlSerializerLC( aSendBuffer );
    GenerateRequestL( *serialize,
                      iFetchedPresenceIds,
                      *iFechedAttributeTypes,
                      iCspVersion );

    CleanupStack::PopAndDestroy(); //serialize
    }



// -----------------------------------------------------------------------------
// CPEngFetchProcessor::InitResponseProcessingL()
// -----------------------------------------------------------------------------
//
void CPEngFetchProcessor::InitResponseProcessingL(
    const TDesC8& aResponse,
    RPointerArray< MPEngPresenceAttrModel2 >& aResultModels,
    MPEngAdvTransactionStatus2& aResultStatus )


    {
    iPresenceBlocks.Reset();
    iFetchedModels = &aResultModels;
    iTransactionStatus = &aResultStatus;


    // If there isn't status primitive or the presence response,
    // indicate the corrupted response to client
    if ( !iXmlParser->DecodeL( aResponse, KStatusTag, ETrue, 3 ) &&
         !iXmlParser->DecodeL( aResponse, KGetPresenceResponse, ETrue ) )
        {
        User::Leave( KErrCorrupt );
        }


    //Is there something to parse ?
    if ( CheckResultFromFromResponseL( aResponse,
                                       *iDomain,
                                       iTransactionOperation,
                                       *iXmlParser,
                                       *iTransactionStatus,
                                       iFetchedPresenceIds
                                     ) )
        {
        //Split presence blocks from response to array
        LocatePresenceBlocksFromResponseL( aResponse,
                                           *iXmlParser,
                                           iPresenceBlocks );
        }
    }



// -----------------------------------------------------------------------------
// CPEngFetchProcessor::ProcessStepL()
// -----------------------------------------------------------------------------
//
TBool CPEngFetchProcessor::ProcessStepL()
    {
    if ( iPresenceBlocks.Count() > 0 )
        {
        //Handle the first block
        ParseModelsFromBlockL( iPresenceBlocks[ 0 ],
                               iAttributeManager,
                               *iXmlParser,
                               iFetchedPresenceIds,
                               *iDomain,
                               *iFechedAttributeTypes,
                               *iFetchedModels,
                               iTransactionOperation );
        //Remove the first block
        iPresenceBlocks.Delete( 0 );
        return ETrue;
        }

    return EFalse;
    }




// -----------------------------------------------------------------------------
// CPEngFetchProcessor::GenerateRequestL()
// Parsing helper
// -----------------------------------------------------------------------------
//
void CPEngFetchProcessor::GenerateRequestL(
    MPEngXMLSerializer& aSerializer,
    const CDesC16ArraySeg& aFetchedPresenceIds,
    const CPEngAttributeTypeStack& aFechedAttributeTypes,
    TPEngWVCspVersion aCspVersion )
    {
    TPtrC8 transactionContentNS( KTransactionContentNS );
    TPtrC8 presenceSubListNS( KPresenceSubListAttributesNS );

    if ( aCspVersion == EWVCspV12 )
        {
        transactionContentNS.Set( KTransactionContentNS_CSP12 );
        presenceSubListNS.Set( KPresenceSubListAttributesNS_CSP12 );
        }

    const TInt presenceIdCount( aFetchedPresenceIds.Count() );
    aSerializer.StartTagL( KTransactionContent );
    aSerializer.AttributeL( KXmlXmlns, transactionContentNS );

    aSerializer.StartTagL( KGetPresenceRequest );

    //Generate first requested presence ID's
    TInt index = 0;
    for ( index = 0; index < presenceIdCount; index++ )
        {
        aSerializer.StartTagL( KUser );
        aSerializer.StartTagL( KUserIDXMLTag );
            {
            aSerializer.WvAddressL( aFetchedPresenceIds[ index ] );
            }
        aSerializer.EndTagL( KUserIDXMLTag );
        aSerializer.EndTagL( KUser );
        }


    //And then requested attribute ID's
    //When engine starts support extension attributes,
    //this must be changed to use properly the name spaces.
    aSerializer.StartTagL( KPresenceSubList );
    aSerializer.AttributeL( KXmlXmlns, presenceSubListNS );

    TInt attributeCount( aFechedAttributeTypes.TypeCount() );
    for ( index = 0; index < attributeCount; index++ )
        {
        aSerializer.StartTagL( aFechedAttributeTypes.Name( index ) );
        aSerializer.EndTagL( aFechedAttributeTypes.Name( index ) );
        }

    aSerializer.EndTagL( KPresenceSubList );

    aSerializer.EndTagL( KGetPresenceRequest );
    aSerializer.EndTagL( KTransactionContent );
    }



// -----------------------------------------------------------------------------
// CPEngFetchProcessor::CheckResultFromFromResponseL()
// Parsing helper
// -----------------------------------------------------------------------------
//
TBool CPEngFetchProcessor::CheckResultFromFromResponseL(
    const TDesC8& aResponse,
    const TDesC& aDomain,
    TInt aTransactionOperation,
    MPEngXMLParser& aParser,
    MPEngAdvTransactionStatus2& aTransactionStatus,
    CDesC16Array& aRequestedPresenceIds )
    {
    //Get the result structure from response
    aParser.ParseResultL( aResponse,
                          aTransactionOperation,
                          aTransactionStatus );

    //Filter presence id's according the network server response status
    //If all requested presence id's were unknown, no need to do further
    //processing. In this case add all requested presence id's
    //as detailed result.
    if ( aTransactionStatus.Status() == KPEngNwErrUnknownUser )
        {
        TInt presIdCount = aRequestedPresenceIds.Count();
        for ( TInt presIdIx( 0 ); presIdIx < presIdCount; presIdIx++ )
            {
            TPtrC presenceId( aRequestedPresenceIds[ presIdIx ] );
            aTransactionStatus.AddDetailedResultL( aTransactionOperation,
                                                   KPEngNwErrUnknownUser,
                                                   NULL,
                                                   &presenceId,
                                                   NULL,
                                                   NULL );
            }

        return EFalse;
        }

    //If partial failure, remove unknown presence id's
    if ( aTransactionStatus.Status() == KPEngNwErrPartiallySuccessful )
        {
        const TInt detailedResults = aTransactionStatus.DetailedResultCount();
        for ( TInt dResultIx( 0 ); dResultIx < detailedResults; dResultIx++ )
            {
            const MPEngDetailedResultEntry2& dEntry =
                aTransactionStatus.DetailedResult( dResultIx );
            TPtrC unknownPresenceId( NULL, 0 );
            if ( dEntry.GetDetailedDesc( unknownPresenceId,
                                         EPEngDTPresenceID ) == KErrNone )
                {
                //there was a presence id reported in detailed result
                //assume that it is failed and try remove it from fetched presences
                TInt index =
                    PEngAddressUtils::FindWVAddressF( unknownPresenceId,
                                                      aRequestedPresenceIds,
                                                      aDomain,
                                                      PEngAddressUtils::EMatchToUniqueId );
                if ( index != KErrNotFound )
                    {
                    aRequestedPresenceIds.Delete( index );
                    }

                //If there is some requested user ID reported in response as unknown,
                //but its locating & remove from requested ID's array fails
                //==> no problem, only those users models are anyway returned
                //to client, which are found from response.
                }
            }
        }

    return ETrue;
    }



// -----------------------------------------------------------------------------
// CPEngFetchProcessor::LocatePresenceBlocksFromResponseL()
// Parsing helper
// -----------------------------------------------------------------------------
//
void CPEngFetchProcessor::LocatePresenceBlocksFromResponseL(
    const TDesC8& aResponse,
    MPEngXMLParser& aParser,
    CDesC8Array& aPresenceBlocks )
    {
    //locate the presence data blocks
    if ( aParser.DecodeL( aResponse, KPresenceXMLTag, ETrue ) )
        {
        //place found presence data blocks to an array
        TInt foundPresencesCount = aParser.Count();
        for ( TInt blockIndex = 0; blockIndex < foundPresencesCount; blockIndex++ )
            {
            aPresenceBlocks.AppendL( aParser.ResultL( blockIndex ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CPEngFetchProcessor::ParseModelsFromBlockL()
// Parsing helper
// -----------------------------------------------------------------------------
//
void CPEngFetchProcessor::ParseModelsFromBlockL(
    const TDesC8& aPresenceBlock,
    MPEngPresenceAttrManager& aAttrManager,
    MPEngXMLParser& aParser,
    CDesC16Array& aFetchedPresenceIds,
    const TDesC& aDomain,
    const CPEngAttributeTypeStack& aAttributeTypes,
    RPointerArray<MPEngPresenceAttrModel2>& aResultModels,
    TInt aOperation )
    {
    if ( aParser.DecodeL( aPresenceBlock, KUserIDXMLTag, EFalse ) )
        {
        //There is a presence id => unescape the id
        HBufC* presIdBuf = aParser.ResultAsWVAddressL();
        CleanupStack::PushL( presIdBuf );

        //Try get the presence id as it was really requested
        TInt presenceIdIndex = PEngAddressUtils::FindWVAddressF( *presIdBuf,
                                                                 aFetchedPresenceIds,
                                                                 aDomain,
                                                                 PEngAddressUtils::EMatchToUniqueId );
        if ( presenceIdIndex != KErrNotFound )
            {
            //There is client requested presence ID for this presence block
            //Use the client given ID as model ID.
            //If not able to match to client given id, actual id from presence block is used
            TPtrC clientPresenceId( aFetchedPresenceIds[ presenceIdIndex ] );
            HBufC* clientPresenceIdBuf = clientPresenceId.AllocL();

            CleanupStack::PopAndDestroy(); //presIdBuf
            CleanupStack::PushL( clientPresenceIdBuf );
            presIdBuf = clientPresenceIdBuf;

            //Remove ID from aFetchedPresenceIds array,
            //so it won't be handled again
            aFetchedPresenceIds.Delete( presenceIdIndex );
            }


        //go trough all requested attributes
        TBool onlineStatFetched( EFalse );
        const TInt attrTypeCount = aAttributeTypes.TypeCount();
        for ( TInt attrTypeIx = 0; attrTypeIx < attrTypeCount; attrTypeIx++ )
            {
            const TUint32 attributeType = aAttributeTypes.TypeId( attrTypeIx );
            if ( aParser.DecodeL( aPresenceBlock,
                                  aAttributeTypes.Name( attrTypeIx ),
                                  ETrue ) )
                {
                if ( attributeType == KUidPrAttrOnlineStatus )
                    {
                    onlineStatFetched = ETrue;
                    }
                //attribute type was found from presence data block
                //Instantiate a model
                MPEngPresenceAttrModel2* model = NULL;
                aAttrManager.LoadAttributeLC( model,
                                              *presIdBuf,
                                              attributeType,
                                              EPEngStorableModel );



                //And let it to do the real parsing
                TPtrC8 attributeData = aParser.ResultL();
                model->Advanced()->DecodeFromXmlL( aParser, attributeData );

                //add the model to results
                aResultModels.AppendL( model );
                CleanupStack::Pop(); //model
                }

            else
                {


                if ( attributeType == KUidPrAttrStatusText )
                    {

                    MPEngPresenceAttrModel2*	model = NULL;
                    aAttrManager.LoadAttributeLC( model,
                                                  *presIdBuf,
                                                  KUidPrAttrStatusText,
                                                  EPEngStorableModel );

                    model->SetDataDesC16L( KNullDesC, EPEngStatusText );

                    PEngAttrModelHelper::AddOrReplaceModelL( aResultModels, *model );
                    //     aResultModels.AppendL( model );
                    CleanupStack::Pop(); //model

                    }
                }
            }
        if ( !onlineStatFetched && ( aOperation == EPEngTransOpAttributeFetchToCache ) )
            {
            MPEngPresenceAttrModel2* model = NULL;
            aAttrManager.LoadAttributeLC( model,
                                          *presIdBuf,
                                          KUidPrAttrOnlineStatus,
                                          EPEngStorableModel );
            //add the model to results
            aResultModels.AppendL( model );
            CleanupStack::Pop(); //model
            }

        CleanupStack::PopAndDestroy( presIdBuf );
        }
    }



//  End of File


