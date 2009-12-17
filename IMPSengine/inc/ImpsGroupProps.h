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
*     WV group properties.
*     WV CSP 1.1 recommends some properties and the classes here provide support
*     for them. Additionally is is possible to create and read extra properties
*     not mentioned in CSP 1.1.
*     Because of any of the predefined properties may be missing, we use
*     the value -1 to denote undefined value.
*
*     Notice that the mutators do not change the values immediately in a remote
*     service, but only after the properties have been assigned to the service
*     through SetGroupPropertiesL method in RImpsGroupClient interface.
*
*
*/


#ifndef CImpsGroupProps_H
#define CImpsGroupProps_H

//  INCLUDES
#include <e32base.h>

// Values for user types 
enum TImpsGroupUsers
    {
    EImpsUserUndef = 0,
    EImpsAdmin, 
    EImpsModerator,
    EImpsOrdinary
    };

// Property set type
enum TImpsPropertyType
    {
    EImpsPropPrivate = 1, 
    EImpsPropCommon
    };

// Property values
enum TImpsPropertyBool
    {
    EImpsPropUndef = -1,    // undefined
    EImpsPropNo = 0,        // false, no, ...
    EImpsPropYes = 1        // true, yes
    };


// CLASS DECLARATION

/**
* CImpsGroupProp
* 
* A single property. used for external properties not mentioned in CSP.
* 
*/

class CImpsGroupProp :public CBase
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aKey property key
        * @param aValue property value
        */
        IMPORT_C static CImpsGroupProp* NewL(
            const TDesC& aKey, 
            const TDesC& aValue );
        
        /**
        * Destructor.
        */
        virtual ~CImpsGroupProp();

    public: // New functions

        /**
        * Key accessor
        * @return key
        */
        IMPORT_C TPtrC Key() const;

        /**
        * Value accessor
        * @return value
        */
        IMPORT_C TPtrC Value() const;

    protected:

         
        /**
        * C++ constructor.
        */
        CImpsGroupProp(  );

    private:

        /**
        * Actual construction.
        * @param aKey property key
        * @param aValue property value
        */
        void ConstructL(
            const TDesC& aKey, 
            const TDesC& aValue );

    public:     // Data

        TDblQueLink iLink;

    private:    // Data

        HBufC*      iKey;
        HBufC*      iValue;

    public:     // Friend classes

        friend class CImpsGroupProps;

    };

/**
* CImpsGroupProps
* 
* WV Group properties base class.
*/
class CImpsGroupProps :public CBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsGroupProps();

        /**
        * Initialize member data
        */
        IMPORT_C virtual void Reset() = 0;

        /**
        * Returns the type of the properties
        */
        IMPORT_C virtual TImpsPropertyType Type() const = 0;

    public: // New functions

        /**
        * Get current number of extra properties
        */
        IMPORT_C TInt Count();

        /**
        * Add new extra property
        * @param aProperty new property. 
        *  The data is copied to internal structures.
        */
        IMPORT_C void AddL( const CImpsGroupProp& aProperty );

        /**
        * Access first extra property
        * Leaves with KErrEof if no data
        * @return first property
        */
        IMPORT_C const CImpsGroupProp& FirstL();

        /**
        * Access next property
        * Leaves with KErrEof if no data
        * @return next property
        */
        IMPORT_C const CImpsGroupProp& NextL();

    protected:

        /**
        * C++ constructor 
        */
        IMPORT_C CImpsGroupProps();

        /**
        * Is private messages allowed
        * @return EImpsPropYes if allowed
        */
        TImpsPropertyBool IsPrivateAllowed() const;

        /**
        * Mutator for private messaging flag
        * @param aValue EImpsPropYes if private messaging allowed
        */
        void SetPrivateAllowed( TImpsPropertyBool aValue );

    private:

        /**
        * Actual construction.
        */
        void ConstructL();
    
    private:
        TBool           iReset;  // Is reset called

    public:     // Data
        // Linked list for extra properties
        TDblQue<CImpsGroupProp> iPropertyList;

        // Pointer to the current value
        TDblQueIter<CImpsGroupProp> iIter;     

    protected:  // Data
        TImpsPropertyBool       iPrivateMessages;  // private messaging allowed

    };

class CImpsPrivateGroupProps :public CImpsGroupProps
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsPrivateGroupProps* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsPrivateGroupProps();
        
        /**
        * Initialize member data
        */
        IMPORT_C void Reset();

        /**
        * Returns the type of the properties
        */
        IMPORT_C virtual TImpsPropertyType Type() const;

        /**
        * Is private messages allowed
        * @return EImpsPropYes if allowed
        */
        IMPORT_C TImpsPropertyBool IsPrivateAllowed() const;

        /**
        * Mutator for private messaging flag
        * @param aValue EImpsPropYes if private messaging allowed
        */
        IMPORT_C void SetPrivateAllowed( TImpsPropertyBool aValue );

        /**
        * Is the user a member
        * @return EImpsPropYes if user is a member
        */
        IMPORT_C TImpsPropertyBool IsMember() const;

        /**
        * Privilege accessor
        * @return user privileges (role) in the group
        */
        IMPORT_C TImpsGroupUsers Privileges() const;




    protected:
        /**
        * C++ constructor.
        */
        IMPORT_C CImpsPrivateGroupProps();
  
        /**
        * Actual construction
        */
        void ConstructL();

    protected:

        TImpsPropertyBool   iMember;        // Am i member
        TImpsGroupUsers     iPrivilege;     // privilege level

        };


class CImpsCommonGroupProps :public CImpsGroupProps
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsCommonGroupProps* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsCommonGroupProps();

        /**
        * Initialize member data
        */
        IMPORT_C void Reset();

        /**
        * Returns the type of the properties
        */
        IMPORT_C virtual TImpsPropertyType Type() const;

        /**
        * Is private messages allowed
        * @return EImpsPropYes if allowed
        */
        IMPORT_C TImpsPropertyBool IsPrivateAllowed() const;

        /**
        * Mutator for private messaging flag
        * @param aValue EImpsPropYes if private messaging allowed
        */
        IMPORT_C void SetPrivateAllowed( TImpsPropertyBool aValue );

        /**
        * Is group open or closed
        * @return EImpsPropYes if open
        */
        IMPORT_C TImpsPropertyBool IsOpen() const;

        /**
        * Access type mutator
        * @param aType EImpsPropYes if open
        */
        IMPORT_C void SetOpen( TImpsPropertyBool aType ); 

        /**
        * Is group public or private
        * @return EImpsPropYes if public
        */
        IMPORT_C TImpsPropertyBool IsPublic() const;

        /**
        * Is group searchable
        * @return EImpsPropYes if searchable
        */
        IMPORT_C TImpsPropertyBool IsSearchable() const;

        /**
        * Access type mutator
        * @param aType EImpsPropYes if searchable
        */
        IMPORT_C void SetSearchable( TImpsPropertyBool aType ); 

        /**
        * Number of current joined active users
        * @return number of users, -1 if not specified
        */
        IMPORT_C TInt NbrOfUsers() const;

        /**
        * Maximum number of active users
        * @return number of users, -1 if not specified
        */
        IMPORT_C TInt MaxNbrOfUsers() const;

        /**
        * Mutator for maximum number of users
        * @param aNbr new maximum number of users. -1 means undefined
        */
        IMPORT_C void SetMaxNbrOfUsers( TInt aNbr );

        /**
        * Name of the group
        * @return name of the group
        */
        IMPORT_C TPtrC GroupName() const;

        /**
        * Group name mutator
        * @param aName new grop name 
        */
        IMPORT_C void SetGroupNameL( const TDesC& aName );

        /**
        * Topic of the group
        * @return name of the group
        */
        IMPORT_C TPtrC Topic() const;

        /**
        * Topic mutator
        * @param aTopic new grop topic description 
        */
        IMPORT_C void SetTopicL( const TDesC& aTopic );

        /**
        * Welcome text of the group
        * @return text
        */
        IMPORT_C TPtrC Welcome() const;

        /**
        * Topic mutator
        * @param aText new group wellcome test 
        */
        IMPORT_C void SetWelcomeL( const TDesC& aText );

        /**
        * AutoDelete for CSP 1.2
        * @return EImpsPropYes if used
        */
        IMPORT_C TImpsPropertyBool AutoDelete() const;
        
        /**
        * Mutator for auto delete flag in CSP 1.2
        * @param aValue EImpsPropYes if used
        */
        IMPORT_C void SetAutoDelete( TImpsPropertyBool aValue );


    protected:
        /**
        * C++ constructor
        */
        IMPORT_C CImpsCommonGroupProps();

    private:
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    protected:

        TImpsPropertyBool   iAccessOpen;  // access type open/closed
        TImpsPropertyBool   iTypePublic; // type public/private
        TImpsPropertyBool   iSearchable;  // is searchable
        TInt                iNbrActive;   // nbr of active users
        TInt                iMaxUsers;    // max nbr of users
        HBufC*              iName;        // name
        HBufC*              iTopic;
        HBufC*              iWelcome;     // welcome note
        TImpsPropertyBool   iAutoDelete;  // auto delete for CSP 1.2
        };


#endif     
            
