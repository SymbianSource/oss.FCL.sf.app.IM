/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Data structures used fro detailed error data in 
*     WV engine API.
*
*/


#ifndef CImpsDetailed_H
#define CImpsDetailed_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class CImpsDetailedResult;

// CLASS DECLARATION

/**
*  Data structure for array of DetailedResults
*/
class CImpsDetailed :public CArrayPtrFlat<CImpsDetailedResult>
    {
    public: 
        /*
        * Constructor
        */
        IMPORT_C CImpsDetailed(TInt aGranularity);
        
        /*
        * Destructor
        */
        IMPORT_C virtual ~CImpsDetailed();

    };


/**
*  Data structure for DetailedResult in CSP primitives
*/
class CImpsDetailedResult :public CBase
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsDetailedResult* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsDetailedResult();

    public: // New functions
        
        /**
        * Error code accessor
        * @return Code
        */
        IMPORT_C TInt Code() const;

        /**
        * Error code mutator
        * @param aCode Code
        */
        IMPORT_C void SetCode( TInt aCode );

        /**
        * Descriptor accessor
        * @return Description
        */
        IMPORT_C TPtrC Descriptor() const;

        /**
        * Descriptor mutator
        * @param aDes textual descriptor of error
        */
        IMPORT_C void SetDescriptorL( TDesC& aDes );

        /**
        * User id accessor
        * @return UserID*
        */
        IMPORT_C CPtrC16Array* UserIds() const;

        /**
        * User id accessor
        * @return UserID*
        */
        IMPORT_C CPtrC16Array* UserIds();

        /**
        * Group id accessor
        * @return GroupID*
        */
        IMPORT_C const CPtrC16Array* GroupIds() const;

        /**
        * Group id accessor
        * @return GroupID*
        */
        IMPORT_C CPtrC16Array* GroupIds();

        /**
        * Message id accessor
        * @return MessageID*
        */
        IMPORT_C const CPtrC16Array* MessageIds() const;

        /**
        * Message id accessor
        * @return MessageID*
        */
        IMPORT_C CPtrC16Array* MessageIds();

        /**
        * ScreenName(SName) accessor
        * Corresponding GroupID is found by SNameGroups using 
        * the same array index
        * @return SName 
        */
        IMPORT_C const CPtrC16Array* SNames() const;

        /**
        * ScreenName(SName) accessor
        * Corresponding GroupID is found by SNameGroups using 
        * the same array index
        * @return SName 
        */
        IMPORT_C CPtrC16Array* SNames();

        /**
        * ScreenName(GroupID) accessor.
        * Corresponding SName is found by SNames using 
        * the same array index
        * @return GroupID*
        */
        IMPORT_C const CPtrC16Array* SNameGroups() const;

        /**
        * ScreenName(GroupID) accessor.
        * Corresponding SName is found by SNames using 
        * the same array index
        * @return GroupID*
        */
        IMPORT_C CPtrC16Array* SNameGroups();

        /**
        * Reset
        */
        IMPORT_C void Reset();

    private:
        /**
        * C++ default constructor.
        */
        CImpsDetailedResult();

        /**
        * By default constructor is private.
        */
        void ConstructL();

    private:    // Data
        TInt                  iCode;          // code
        TPtrC                 iDescription;   // description, just a reference
        CPtrC16Array*         iUserIds;       // user-Id
        CPtrC16Array*         iGroupIds;      // group-id
        CPtrC16Array*         iMessageIds;    // Message-id
        CPtrC16Array*         iScreenNames;   // Screename::Sname
        CPtrC16Array*         iGroupNames;    // ScreenName::GroupId

    };

#endif     
            
// End of File
