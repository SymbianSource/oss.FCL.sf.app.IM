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
* Description:  Holds error message information
*
*/


#ifndef CCAERRORDATA_H
#define CCAERRORDATA_H

// INCLUDES
#include    <e32base.h>

#include    "MCAError.h"

// FORWARD DECLARATIONS
class       CImpsDetailedResult;

// CLASS DECLARATION

/**
*  CCAErrorData holds the message information.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCAErrorData : public CBase,
            public MCAError
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aDetailedResults defines the error data that will be copied
        * to internal data structures.
        */
        static CCAErrorData* NewL(
            const CImpsDetailedResult& aDetailedResult );

        /**
        * Two-phased constructor. More wider API for testing purposes.
        * @param aCode specifies the additional error code.
        * @param aDescription is the textual description.
        * @param aUserIDs specifies user ID list.
        * @param aGroupIds specifies group ID list.
        * @param aMessageIDs specifies message IDs.
        * @param aScreenNames specify the used screen names in the
        * corresponding groups i.e. aScreenNameGroup.
        * @param aScreeNameGroup specifies the groups for the screen names.
        * Index 0 in the group array corresponds to the screen name in
        * screen name array index 0.
        */
        static CCAErrorData* NewL(
            TInt                aCode,
            const TDesC&        aDescription,
            const CPtrC16Array* aUserIDs,
            const CPtrC16Array* aGroupIDs,
            const CPtrC16Array* aMessageIDs,
            const CPtrC16Array* aScreenNames,
            const CPtrC16Array* aScreenNameGroup );


        /**
        * Destructor.
        */
        virtual ~CCAErrorData();

    public: // Functions derived from MCAError

        /**
        * Returns the error code.
        * @return Error code.
        */
        IMPORT_C TInt Code() const;

        /**
        * Textual description from the error.
        * @return Plain text explanation.
        */
        IMPORT_C const TDesC& Description() const;

        /**
        * Return user IDs.
        * @return Array that holds the user IDs.
        */
        IMPORT_C const CDesCArray& UserIDs() const;

        /**
        * Return group IDs.
        * @return Array that holds the group IDs.
        */
        IMPORT_C const CDesCArray& GroupIDs() const;

        /**
        * Return message IDs.
        * @return Array that holds the message IDs.
        */
        IMPORT_C const CDesCArray& MessageIDs() const;

        /**
        * Return screen names. The corresponding group is found from
        * corresponding array index in the ScreenNameGroup().
        * @return Array that holds the screen names.
        */
        IMPORT_C const CDesCArray& ScreenNames() const;

        /**
        * Return group IDs.
        * @return Array that holds the group IDs.
        */
        IMPORT_C const CDesCArray& ScreenNameGroup() const;


    private:

        /**
        * By default Symbian OS constructor is private.
        * @see CCAErrorData::NewL.
        */
        void ConstructL( const CImpsDetailedResult& aDetailedResult );

        /**
        * By default Symbian OS constructor is private.
        * @see CCAErrorData::NewL.
        */
        void ConstructL(
            TInt                aCode,
            const TDesC&        aDescription,
            const CPtrC16Array* aUserIDs,
            const CPtrC16Array* aGroupIDs,
            const CPtrC16Array* aMessageIDs,
            const CPtrC16Array* aScreenNames,
            const CPtrC16Array* aScreenNameGroup );

        /**
        * C++ default constructor.
        */
        CCAErrorData();

        /**
        * Sets the error code value.
        * @param aCode specifies the error code.
        */
        void SetCode( TInt aCode  );

        /**
        * Sets the error user IDs.
        * @param aUserIDs specify array of User IDs.
        */
        void SetUserIDsL( const CPtrC16Array* aUserIDs );

        /**
        * Sets the group IDs.
        * @param aGroupIDs specify array of group IDs.
        */
        void SetGroupIDsL( const CPtrC16Array* aGroupIDs );

        /**
        * Sets the message IDs.
        * @param aMessageIDs specify the array of message IDs.
        */
        void SetMessageIDsL( const CPtrC16Array* aMessageIDs );

        /**
        * Sets the screen names.
        * @param aScreenNames specify the array of screen names.
        */
        void SetScreenNamesL( const CPtrC16Array* aScreenNames );

        /**
        * Sets the screen name groups.
        * @param aScreenNameGroups specify the groups in a such
        * way that the corresponding screen name is found from the
        * screen name array with the corresponding array index.
        */
        void SetScreenNamesGroupsL( const CPtrC16Array* aScreenNameGroups );

    private:    // Data

        // Specifies the error code
        TInt iCode;

        // Owns: Textual description from the error
        HBufC* iDescription;

        // Owns: user WV IDs
        CDesCArrayFlat* iUserIDs;

        // Owns: group WV IDs
        CDesCArrayFlat* iGroupIDs;

        // Owns: Message IDs
        CDesCArrayFlat* iMsgIDs;

        // Owns: Screen names
        CDesCArrayFlat* iScreenNames;

        // Owns: Screen name in group the corresponding screen name
        // is found with the same index from the iScreenNames
        CDesCArrayFlat* iScreenNameGroup;

    };

#endif      // CCAERRORDATA_H     

// End of File
