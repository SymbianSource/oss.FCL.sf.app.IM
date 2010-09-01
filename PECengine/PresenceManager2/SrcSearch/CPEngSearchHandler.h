/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class handls the search operation
*
*/

#ifndef CPENGSEARCHHANDLER_H
#define CPENGSEARCHHANDLER_H

//  INCLUDES
#include <E32Base.h>
#include <ImpsClient.h>
#include <ImpsFundCli.h>
#include <ImpsFundamental.h>



// FORWARD DECLARATIONS
class CPEngSearchCriteria2;
class CPEngSearchResultEntry;
class RImpsEng;


/**
 *
 *
 * @lib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSearchHandler ) : public CBase,
        public MImpsSearchHandler2,
        public MImpsErrorHandler2


    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngSearchHandler* NewL( TInt aPriority,
    RImpsEng& aImpsEngine,
    CPEngSearchResultEntry& aResultContainer,
    const RPointerArray< CPEngSearchCriteria2 >& aCriterias,
    TInt aSearchLimit );

    /**
     * Destructor.
     */
    virtual ~CPEngSearchHandler();


private:

    /**
     * C++ default constructor.
     */
    CPEngSearchHandler( CPEngSearchResultEntry& aResultContainer,
                        TInt aSearchLimit );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( TInt aPriority,
                     RImpsEng& aImpsEngine,
                     const RPointerArray< CPEngSearchCriteria2 >& aCriterias );




public: //New functions


    void SearchL( TRequestStatus& aStatus );

    void ContinueSearchL( TInt aContinueIndex,
                          TRequestStatus& aStatus );

    void StopSearchL( TRequestStatus& aStatus );

    TBool IsRequestPending( TInt aId );



private: //From MImpsSearchHandler2

    /**
     * Observer method for search response
     *
     * @param aOpId operation id returned by SearchFirstL or SearchNextL.
     * @param aSearchID search operation id,
     *        used when search is continued or stopped.
     * @param aIndex indicates the particular index from which the
     *        next search can start. It is 0 in error case.
     * @param aResultsFound indicates the number of the current findings.
     * @param aCompleted TRUE if server has completed the search,
     *        FALSE if there are more results available.
     * @param aType which kind of items there are in aResults parameter
     * @param aResults list of results,
     *        NULL if none or aStatus has an error code.
     * @param aCspId CSP session identifier
     */
    void HandleSearchL( TInt aOpId,
                        TInt aSearchId,
                        TInt aIndex,
                        TInt aResultsFound,
                        TBool aCompleted,
                        TImpsSearchResultType aType,
                        MDesCArray* aResults ,
                        TImpsCspIdentifier& aCspId );


    /**
     * Observer for stopped search
     * This is called when WV server has stopped the search after StopSearchL.
     * Notice that a previous search is invalidated
     * when a new search is started,
     * but this method is not called in that situation.
     * @param aOpId operation id returned by StopSearchL.
     * @param aSearchID search operation id,
     *        used when search was stopped.
     * @param aCspId CSP session identifier
     */
    void HandleSearchStoppedL( TInt aOpId,
                               TInt aSearchId,
                               TImpsCspIdentifier& aCspId );



private: //From MImpsErrorHandler2
    /**
     * Observer method for error messages.
     * This provides data for partial error responses too.
     * Each service API MUST register this observer by RegisterErrorObserverL
     * immediately after Register method.
     * @param aStatus error code.
     * @param aOpId operation id to map responses to the requests.
     * @param aDescripton error description
     * @param aDetailedRes detailed error description
     * @param aCspId CSP session identifier
     */
    void HandleErrorL( TInt aStatus,
                       TInt aOpId,
                       const TDesC* aDescription,
                       const CImpsDetailed* aDetailedRes,
                       TImpsCspIdentifier& aCspId );


private: //Data

    //REF:
    CPEngSearchResultEntry& iResultContainer;

    //OWN:
    const TInt  iSearchLimit;

    //OWN:
    CSearchPairs*   iSearchPairs;

    //OWN:
    RImpsFundClient2    iFundClient;


    TInt    iOpId;
    TInt    iSearchId;
    TRequestStatus*     iRequest;
    };

#endif  //CPENGSEARCHHANDLER_H



