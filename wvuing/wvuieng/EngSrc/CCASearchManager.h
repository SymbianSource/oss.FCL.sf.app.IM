/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class offers the search services
 *
*/



#ifndef CCASEARCHMANAGER_H
#define CCASEARCHMANAGER_H

// INCLUDES
#include "MCASearchInterface.h"
#include "MCAImpsFundClient.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class CCARequestMapper;
class CCASearchData;
class MCAImpsFundClient;
class MCAImpsFactory;
class MCASearchObserverInterface;

// CLASS DECLARATION

/**
 *  This class offers the search services.
 *
 *  @lib CAEngine.lib
 *  @since 2.1
 */
class CCASearchManager : public CBase,
            public MCASearchInterface,
            public MImpsSearchHandler2
    {
    private:  // Enumerations

        // Search states
        enum TSearchState
            {
            ENoActivity             = 0,
            ESearchStartRequested   = 1,
            ESearchGoingOn          = 2
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aImpsFundAPI Interface of wrapped Imps fundamental client
         * @param aRequestMapper Interface of request mapper
         */
        static CCASearchManager* NewL(
            MCAImpsFactory* aIMPSFactory,
            CCARequestMapper* aRequestMapper );

        /**
         * Destructor.
         */
        virtual ~CCASearchManager();

    public: // New functions

        /**
         * Sets the search support flag.
         * @param aSearchSupported specifies whether the search support is on
         * or not.
         */
        void SetSearchSupported( TBool aSearchSupported );

    public: // Functions derived from MCASearchInterface

        /**
         * From MCASearchInterface.
         * @see MCASearchInterface::StartSearchL.
         */
        IMPORT_C TInt StartSearchL(
            const CSearchPairs& aPairs,
            TInt aSearchLimit,
            MCASearchObserverInterface* aObserver
        );

        /**
         * From MCASearchInterface.
         * @see MCASearchInterface::EndSearchL.
         */
        IMPORT_C TInt EndSearchL();

        /**
         * From MCASearchInterface.
         * @see MCASearchInterface::SearchNextL.
         */
        IMPORT_C TInt SearchNextL( TInt aIndex,
                                   MCASearchObserverInterface* aObserver
                                 );

        /**
         * From MCASearchInterface.
         * @see MCASearchInterface::SearchDataInterface.
         */
        IMPORT_C MCASearchData* SearchDataInterface() const;

    private: // Functions derived from MImpsSearchHandler

        /**
         * @see MImpsSearchHandler2
         */
        void HandleSearchL( TInt aId,
                            TInt aSearchId,
                            TInt aIndex,
                            TInt aResultsFound,
                            TBool aCompleted,
                            TImpsSearchResultType aType,
                            MDesCArray* aResults ,
                            TImpsCspIdentifier& aCspId );

        /**
         * @see MImpsSearchHandler2
         */
        void HandleSearchStoppedL( TInt aId,
                                   TInt aSearchId,
                                   TImpsCspIdentifier& aCspId );

    private:

        /**
         * C++ default constructor.
         */
        CCASearchManager(
            MCAImpsFactory* aIMPSFactory,
            CCARequestMapper*   aRequestMapper );

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

        /**
         * Saves the results obtained from the search so that we do not
         * have to obtain them everytime.
         * @see CCASearchManager::HandleSearchL.
         * Leaves
         *   EECouldntSaveSearchResults if unable to save results.
         */
        void SaveResultsL( TInt aIndex,
                           TBool aCompleted,
                           TImpsSearchResultType aType,
                           MDesCArray* aResults );

    private:    // Data

        // not owned
        MCAImpsFactory* iImpsFactory;

        // Not owned: API to the search interface
        MCAImpsFundClient* iImpsFundClient;

        // Owns: Results
        CCASearchData* iResults;

        // 10 digit search id
        TInt iSearchID;

        // State of the search
        TSearchState iState;

        // Search limit I.e. results to the search observer.
        TInt iSearchLimit;

        // Doesn't own. Synchronous request mapper.
        CCARequestMapper* iRequestMapper;

        // Owns. Array of groups created by given user
        CDesCArray* iCreatedGroups;

        TBool iOwnGrouplistReady;
        // Owns. wvid of user whose grouplist is cached
        HBufC* iOwnGrouplistCreatorId;

        // Does the server support searching.
        TBool iSearchSupported;

        // current observer
        MCASearchObserverInterface* iObserver;      // not owned
    };

#endif      // CCASEARCHMANAGER_H

// End of File
