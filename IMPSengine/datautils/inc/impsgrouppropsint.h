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
*     Internal group property mutators for engine's internal use, not included
*     in the group client API for read-only properties.
*
*     DO NOT DEFINE your own data members in these classes. Otherwise the cast
*     operation is not possible.
*
*
*/


#ifndef ImpsInternalProperties_H
#define ImpsInternalProperties_H

//  INCLUDES
#include "impsgroupprops.h"

// CONSTANTS

// size of mem::copy for CImpsGroupProps
// 1 * 4
const TInt KImpsPackedProps = 4;

// size of mem::copy for CImpsCommonGroupPropsInt
// 5 * 4
const TInt KImpsPackedCommonProps = 20;
// Nbr of fields that have streamed
const TInt KImpsPackedCommonPropsNbr = 3;

// size of mem::copy for CImpsPrivateGroupPropsInt
// 2 * 4
const TInt KImpsPackedPrivateProps = 8;
// Nbr of fields that have streamed
const TInt KImpsPackedPrivatePropsNbr = 0;


// CLASS DECLARATION

//***********************************************************
// Internal common group property mutators.
// ***********************************************************

class CImpsCommonGroupPropsInt : public CImpsCommonGroupProps
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsCommonGroupPropsInt* NewL();

        /**
        * Destructor.
        */
        virtual ~CImpsCommonGroupPropsInt();

        /**
        * Reset
        */
        IMPORT_C void Reset();

        /**
        * Access type mutator
        * @param aType EImpsPropYes if public
        */
        IMPORT_C void SetPublic( TImpsPropertyBool aType );

        /**
        * Mutator for current number of users
        * @param aNbr number of users. -1 means undefined
        */
        IMPORT_C void SetNbrOfUsers( TInt aNbr );

        /**
        * Size accessor
        * @return size of this entity
        */
        TInt Size() const;


        /**
        * Is there any real data
        * @return TBool if there is data
        */
        TBool HasData() const;

    private:
        /**
        * By default Symbian OS constructor is private.
        */
        CImpsCommonGroupPropsInt();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     // Friend classes
        friend class TImpsPackedEntity;
        friend class CImpsSession;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes

    };

//***********************************************************
// Internal private group property mutators.
//***********************************************************

class CImpsPrivateGroupPropsInt : public CImpsPrivateGroupProps
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsPrivateGroupPropsInt* NewL();

        /**
        * Destructor.
        */
        virtual ~CImpsPrivateGroupPropsInt();

        /**
        * Reset
        */
        IMPORT_C void Reset();

        /**
        * Mutator for user membership
        * @param aValue EImpsPropYes if user is a member
        */
        IMPORT_C void SetMember( TImpsPropertyBool aValue );

        /**
        * Privilege mutator
        * @param aValue privileges (role) in the group
        */
        IMPORT_C void SetPrivileges( TImpsGroupUsers aValue );

        /**
        * Size accessor
        * @return size of this entity.
        */
        TInt Size() const;

        /**
        * Is there any real data
        * @return TBool if there is data
        */
        TBool HasData() const;

    private:
        /**
        * By default Symbian OS constructor is private.
        */
        CImpsPrivateGroupPropsInt();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     // Friend classes
        friend class TImpsPackedEntity;

    };

#endif      // ?INCLUDE_H   

// End of File
